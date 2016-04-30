def app(env, start_response):
    status = '200 OK'
    response_headers = []
    start_response(status, response_headers)
    return "Hello World"

def app2(env):
    status = '200 OK'
    response_headers = []
    #start_response(status, response_headers)
    return "Hello World"
