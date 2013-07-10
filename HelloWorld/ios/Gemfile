source 'https://rubygems.org'

gem 'rails', '3.2.12'
gem 'mysql2'
gem 'bcrypt-ruby'
gem 'jquery-rails'
gem 'haml'
gem 'cancan'
gem 'less-rails-bootstrap'
gem 'carrierwave'
gem 'zencoder'
gem 'fog', '~> 1.3.1'
gem 'mini_magick', '3.4'
gem 'param_protected', '~> 4.0.0'
gem 'jquery-fileupload-rails', :git => 'git://github.com/whitmanc/jquery-fileupload-rails.git', :ref => 'e07e8fdff79922b8c72fc78c28933961ea1a370e'
gem 'unicorn'
gem 'therubyracer', '~> 0.10.2'
gem 'barista'
gem 'ttfunk'
gem 'resque'
gem 'resque-loner'
gem 'god'
gem 'yaml_db'

group :osx do
  gem 'betabuilder', :git => 'git://github.com/waseem/betabuilder.git', :branch => 'no_deploy', :ref => '1125dab2d903fb24aabef758d0f1feb22940ed6a'
end

group :assets do
  gem 'less-rails'
  gem 'coffee-rails', '~> 3.2.1'
  gem 'uglifier', '>= 1.0.3'
  gem 'haml_coffee_assets'
  gem 'execjs'
end

group :development, :test do
  gem 'jasminerice'

  gem 'guard'
  gem 'guard-jasmine'
  gem 'guard-rspec'
  gem 'rb-fsevent', '~> 0.9'
end

group :development do
  gem 'pry'
  gem 'pry-rails'
  gem 'haml-rails'
  gem 'capistrano'
  gem 'debugger'
  gem 'growl'
  gem 'zencoder-fetcher'
end

group :test do
  gem 'rspec-rails'
  gem 'shoulda'
  gem 'shoulda-matchers'
  gem 'poltergeist'
  gem 'miniskirt'
  gem 'database_cleaner'
  gem 'ffaker'
  gem 'forgery'
  gem 'launchy'
  gem 'connection_pool'
end

group :deployment do
  gem 'capistrano'
  gem 'rvm-capistrano'
end
