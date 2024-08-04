document.getElementById('loginForm').addEventListener('submit', async function(event) {
    event.preventDefault(); // Prevent the form from submitting the traditional way

    const username = document.getElementById('username').value;
    const password = document.getElementById('password').value;

    try {
        const response = await sendLoginRequest(username, password);
        if (response.redirected) {
            // If the response is redirected, navigate to the new location
            window.location.href = response.url;
        } else {
            const result = await response.json();
            console.log('Response:', result);

            if (response.ok) {
                alert('Login successful');
                window.location.href = '/'; // Redirect to home page on successful login
            } else {
                alert('Login failed: ' + result.message);
            }
        }
    } catch (error) {
        console.error('Error:', error);
        alert('An error occurred');
    }
});

async function sendLoginRequest(username, password) {
    const url = '/login'; // Adjust the URL as needed
    const body = JSON.stringify({ username, password });

    const response = await fetch(url, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: body
    });

    return response;
}
