// script.js - A fun, self-contained test for your webserv + CGI
(function () {
    'use strict';

    // Create a test UI if not already present
    if (document.getElementById('webserv-test-ui')) return;

    const container = document.createElement('div');
    container.id = 'webserv-test-ui';
    container.style.cssText = `
        position: fixed;
        bottom: 10px;
        right: 10px;
        background: #2d2d2d;
        color: #fff;
        padding: 12px;
        border-radius: 8px;
        font-family: monospace;
        max-width: 300px;
        z-index: 10000;
        box-shadow: 0 0 10px rgba(0,0,0,0.5);
    `;

    container.innerHTML = `
        <h3 style="margin:0 0 8px; font-size:14px;">🚀 Webserv CGI Tester</h3>
        <button id="btn-get" style="margin-right:6px;">GET /hello.py</button>
        <button id="btn-post" style="margin-right:6px;">POST to CGI</button>
        <button id="btn-clear">Clear</button>
        <pre id="output" style="margin-top:8px; background:#1e1e1e; padding:6px; height:120px; overflow:auto; white-space:pre-wrap;"></pre>
    `;

    document.body.appendChild(container);

    const output = document.getElementById('output');

    function log(msg) {
        output.textContent += '\n' + msg;
        output.scrollTop = output.scrollHeight;
    }

    function sendRequest(method, body = null) {
        const xhr = new XMLHttpRequest();
        xhr.open(method, '/hello.py', true);
        xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
        xhr.onreadystatechange = function () {
            if (xhr.readyState === 4) {
                if (xhr.status === 200) {
                    // Extract a snippet from the HTML response
                    const doc = new DOMParser().parseFromString(xhr.responseText, 'text/html');
                    const title = doc.querySelector('title')?.textContent || 'No title';
                    const methodFromCGI = doc.querySelector('pre')?.textContent.split('\n')[1] || '';
                    log(`✅ ${method} → ${title}`);
                    log(`   → ${methodFromCGI.trim()}`);
                } else {
                    log(`❌ ${method} failed: ${xhr.status}`);
                }
            }
        };
        xhr.send(body);
    }

    document.getElementById('btn-get').onclick = () => {
        log('Sending GET request...');
        sendRequest('GET');
    };

    document.getElementById('btn-post').onclick = () => {
        const data = `time=${new Date().toISOString()}&client=webserv-js-tester`;
        log('Sending POST request...');
        sendRequest('POST', data);
    };

    document.getElementById('btn-clear').onclick = () => {
        output.textContent = '';
    };

    log('Intialized. Ready to test your webserv + CGI!');
})();
