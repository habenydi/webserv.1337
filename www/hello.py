#!/usr/bin/env python3
import os
import sys

# Get environment variables (required by CGI spec)
method = os.environ.get('REQUEST_METHOD', 'GET')
script_name = os.environ.get('SCRIPT_NAME', '/hello.py')
query_string = os.environ.get('QUERY_STRING', '')
content_length = os.environ.get('CONTENT_LENGTH', '0')

# Read request body if POST
body = ""
if method == "POST" and content_length.isdigit():
    try:
        body_size = int(content_length)
        body = sys.stdin.read(body_size)
    except:
        body = "(Error reading body)"

# Build response
print("Content-Type: text/html; charset=utf-8")
print()  # blank line = end of headers

print(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Hello from CGI!</title>
    <style>
        body {{ font-family: monospace; background: #f0f0f0; padding: 20px; }}
        .box {{ background: white; border: 2px solid #333; padding: 15px; margin: 10px; }}
        pre {{ background: #eee; padding: 10px; overflow: auto; }}
    </style>
</head>
<body>

<div class="box">
    <h1>🎉 Hello from CGI!</h1>
    <p>You are running this via <strong>your own web server</strong> — congratulations!</p>
    <pre>
    Method: {method}
    Path:   {script_name}
    Query:  {query_string}
    Body:   {body[:200]}... (truncated)
    </pre>
</div>

<div class="box">
    <h2>ASCII Art: Your Server is Alive!</h2>
    <pre>
       ____
      /    \\
     |      |
     |  o  o  |
     |   --   |
      \\____/
        |||
        |||
    </pre>
</div>

<div class="box">
    <h2>How to Test This</h2>
    <ol>
        <li>Start your server: <code>./webserv config/default.conf</code></li>
        <li>Visit: <code>http://localhost:8080/hello.py</code></li>
        <li>Try POST: <code>curl -X POST -d "name=alice" http://localhost:8080/hello.py</code></li>
    </ol>
</div>

</body>
</html>
""")
