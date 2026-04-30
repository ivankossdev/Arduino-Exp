from flask import Flask, request

app = Flask(__name__)

@app.route('/auth')
def authRouteHandler():
    auth = request.authorization
    if auth:
        print("Username:", auth.username)
        print("Password:", auth.password)
        return "Authentication successful"
    else:
        return "Unauthorized", 401

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8090)
