package com.startupminds;

import org.cocos2dx.dontsticksticks.R;
import org.cocos2dx.dontsticksticks.R.id;
import org.cocos2dx.dontsticksticks.R.layout;

import android.app.Activity;
import android.content.res.Resources;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Bundle;
import android.widget.MediaController;
import android.widget.VideoView;

public class VideoPlayer extends Activity {
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.videoplayer);
		
		VideoView videoView = (VideoView)findViewById(R.id.VideoView);
		
		boolean showControl = this.getIntent().getBooleanExtra("showControl", true);
		String fileName = this.getIntent().getStringExtra("fileName");
		
		if (showControl) {
			MediaController mediaController = new MediaController(this);
			mediaController.setAnchorView(videoView);
			videoView.setMediaController(mediaController);
		}		
		
		// remove file type
		fileName = fileName.substring(0, fileName.indexOf("."));
		
		// set uri
		Resources res = this.getResources();		
		int videoId = res.getIdentifier(this.getPackageName() + ":raw/" + fileName, null, null);		
		String uri = "android.resource://" + getPackageName() + "/" + videoId;
		videoView.setVideoURI(Uri.parse(uri));
		
		videoView.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
			
			@Override
			public void onCompletion(MediaPlayer mp) {
				// TODO Auto-generated method stub
				VideoPlayer.this.finish();
			}
		});
		
		videoView.start();
	}
	
	public void onPause() {
		super.onPause();
		
		this.finish();
	}
}
