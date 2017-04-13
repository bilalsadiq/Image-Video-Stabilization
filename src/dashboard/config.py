class BaseConfig(object):
    PORT = 80
    HOST = '0.0.0.0'

class DevConfig(BaseConfig):
    DEBUG = True
    PORT = 8080
    
