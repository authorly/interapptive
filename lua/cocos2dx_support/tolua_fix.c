
#include "tolua_fix.h"
#include <stdlib.h>

TOLUA_API void tolua_prepare_ccobject_table(lua_State* L)
{
    lua_pushstring(L, TOLUA_REFID_PTR_MAPPING);
    lua_newtable(L);
    lua_rawset(L, LUA_REGISTRYINDEX);
    
    lua_pushstring(L, TOLUA_REFID_TYPE_MAPPING);
    lua_newtable(L);
    lua_rawset(L, LUA_REGISTRYINDEX);
    
    lua_pushstring(L, TOLUA_REFID_FUNC_MAPPING);
    lua_newtable(L);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

TOLUA_API int tolua_pushusertype_ccobject(lua_State* L,
                                          int refid,
                                          int* p_refid,
                                          void* ptr,
                                          const char* type)
{
    if (ptr == NULL || p_refid == NULL)
    {
        lua_pushnil(L);
        return -1;
    }
    
    if (*p_refid == 0)
    {
        *p_refid = refid;
        
        lua_pushstring(L, TOLUA_REFID_PTR_MAPPING);
        lua_rawget(L, LUA_REGISTRYINDEX);                           /* stack: refid_ptr */
        lua_pushinteger(L, refid);                                  /* stack: refid_ptr refid */
        lua_pushlightuserdata(L, ptr);                              /* stack: refid_ptr refid ptr */
        
        lua_rawset(L, -3);                  /* refid_ptr[refid] = ptr, stack: refid_ptr */
        lua_pop(L, 1);                                              /* stack: - */
        
        lua_pushstring(L, TOLUA_REFID_TYPE_MAPPING);
        lua_rawget(L, LUA_REGISTRYINDEX);                           /* stack: refid_type */
        lua_pushinteger(L, refid);                                  /* stack: refid_type refid */
        lua_pushstring(L, type);                                    /* stack: refid_type refid type */
        lua_rawset(L, -3);                /* refid_type[refid] = type, stack: refid_type */
        lua_pop(L, 1);                                              /* stack: - */
        
        //printf("[LUA] push CCObject OK - refid: %d, ptr: %x, type: %s\n", *p_refid, (int)ptr, type);
    }
    
    tolua_pushusertype(L, ptr, type);
    return 0;
}

TOLUA_API int tolua_remove_ccobject_by_refid(lua_State* L, int refid)
{
	void* ptr = NULL;
    const char* type = NULL;
    void** ud = NULL;
    if (refid == 0) return -1;
    
    // get ptr from tolua_refid_ptr_mapping
    lua_pushstring(L, TOLUA_REFID_PTR_MAPPING);
    lua_rawget(L, LUA_REGISTRYINDEX);                               /* stack: refid_ptr */
    lua_pushinteger(L, refid);                                      /* stack: refid_ptr refid */
    lua_rawget(L, -2);                                              /* stack: refid_ptr ptr */
    ptr = lua_touserdata(L, -1);
    lua_pop(L, 1);                                                  /* stack: refid_ptr */
    if (ptr == NULL)
    {
        lua_pop(L, 1);
        // Lua stack has closed, C++ object not in Lua.
        // printf("[LUA ERROR] remove CCObject with NULL ptr, refid: %d\n", refid);
        return -2;
    }
    
    // remove ptr from tolua_refid_ptr_mapping
    lua_pushinteger(L, refid);                                      /* stack: refid_ptr refid */
    lua_pushnil(L);                                                 /* stack: refid_ptr refid nil */
    lua_rawset(L, -3);                     /* delete refid_ptr[refid], stack: refid_ptr */
    lua_pop(L, 1);                                                  /* stack: - */
    
    
    // get type from tolua_refid_type_mapping
    lua_pushstring(L, TOLUA_REFID_TYPE_MAPPING);
    lua_rawget(L, LUA_REGISTRYINDEX);                               /* stack: refid_type */
    lua_pushinteger(L, refid);                                      /* stack: refid_type refid */
    lua_rawget(L, -2);                                              /* stack: refid_type type */
    if (lua_isnil(L, -1))
    {
        lua_pop(L, 2);
        printf("[LUA ERROR] remove CCObject with NULL type, refid: %d, ptr: %x\n", refid, (int)ptr);
        return -1;
    }
    
    type = lua_tostring(L, -1);
    lua_pop(L, 1);                                                  /* stack: refid_type */
    
    // remove type from tolua_refid_type_mapping
    lua_pushinteger(L, refid);                                      /* stack: refid_type refid */
    lua_pushnil(L);                                                 /* stack: refid_type refid nil */
    lua_rawset(L, -3);                    /* delete refid_type[refid], stack: refid_type */
    lua_pop(L, 1);                                                  /* stack: - */
    
    // get ubox
    luaL_getmetatable(L, type);                                     /* stack: mt */
    lua_pushstring(L, "tolua_ubox");                                /* stack: mt key */
    lua_rawget(L, -2);                                              /* stack: mt ubox */
    if (lua_isnil(L, -1))
    {
        // use global ubox
        lua_pop(L, 1);                                              /* stack: mt */
        lua_pushstring(L, "tolua_ubox");                            /* stack: mt key */
        lua_rawget(L, LUA_REGISTRYINDEX);                           /* stack: mt ubox */
    };
    
    lua_pushlightuserdata(L, ptr);                                  /* stack: mt ubox ptr */
    lua_rawget(L,-2);                                               /* stack: mt ubox ud */
    if (lua_isnil(L, -1))
    {
        // Lua object has released (GC), C++ object not in ubox.
        //printf("[LUA ERROR] remove CCObject with NULL ubox, refid: %d, ptr: %x, type: %s\n", refid, (int)ptr, type);
        lua_pop(L, 3);
        return -3;
    }
    
    ud = (void**)lua_touserdata(L, -1);
    lua_pop(L, 1);                                                  /* stack: mt ubox */
    if (ud == NULL)
    {
        printf("[LUA ERROR] remove CCObject with NULL userdata, refid: %d, ptr: %x, type: %s\n", refid, (int)ptr, type);
        lua_pop(L, 2);
        return -1;
    }
    
    // clean userdata
    *ud = NULL;
    
    lua_pushlightuserdata(L, ptr);                                  /* stack: mt ubox ptr */
    lua_pushnil(L);                                                 /* stack: mt ubox ptr nil */
    lua_rawset(L, -3);                             /* ubox[ptr] = nil, stack: mt ubox */
    
    lua_pop(L, 2);
    //printf("[LUA] remove CCObject, refid: %d, ptr: %x, type: %s\n", refid, (int)ptr, type);
    return 0;
}

TOLUA_API int tolua_ref_function(lua_State* L, int lo, int def)
{
    static int functionRefIDCount = 0;
    if (lua_gettop(L) < abs(lo) || !lua_isfunction(L, lo)) return 0;
    
    ++functionRefIDCount;
                                                                    /* stack: ... func ... */
    lua_pushstring(L, TOLUA_REFID_FUNC_MAPPING);
    lua_rawget(L, LUA_REGISTRYINDEX);                               /* stack: ... func ... refid_func */
    lua_pushinteger(L, functionRefIDCount);                         /* stack: ... func ... refid_func refid */
    lua_pushvalue(L, lo);                                           /* stack: ... func ... refid_func refid func */
    lua_rawset(L, -3);                    /* refid_func[refid] = func, stack: ... func ... refid_func */
    lua_pop(L, 1);                                                  /* stack: ... func ... */

    return functionRefIDCount;
}

TOLUA_API void tolua_remove_function_by_refid(lua_State* L, int refid)
{
    lua_pushstring(L, TOLUA_REFID_FUNC_MAPPING);
    lua_rawget(L, LUA_REGISTRYINDEX);                               /* stack: refid_func */
    lua_pushinteger(L, refid);                                      /* stack: refid_func refid */
    lua_pushnil(L);                                                 /* stack: refid_func refid nil */
    lua_rawget(L, -2);                    /* delete refid_func[refid], stack: refid_func */
    lua_pop(L, 1);                                                  /* stack: - */
}

// check lua value is funciton
TOLUA_API int tolua_isfunction(lua_State* L, int lo, tolua_Error* err)
{
    if (lua_gettop(L) >= abs(lo) && lua_isfunction(L, lo))
    {
        return 1;
    }
    err->index = lo;
    err->array = 0;
    err->type = "[not function]";
    return 0;
}

TOLUA_API void tolua_stack_dump(lua_State* L, const char* label)
{
    int i;
    int top = lua_gettop(L);
    printf("Total [%d] in lua stack: %s\n", top, label != 0 ? label : "");
    for (i = -1; i >= -top; i--)
    {
        int t = lua_type(L, i);
        switch (t)
        {
            case LUA_TSTRING:
                printf("  [%02d] string %s\n", i, lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                printf("  [%02d] boolean %s\n", i, lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                printf("  [%02d] number %g\n", i, lua_tonumber(L, i));
                break;
            default:
                printf("  [%02d] %s\n", i, lua_typename(L, t));
        }
    }
    printf("\n");
}
