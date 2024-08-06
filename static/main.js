document.addEventListener('DOMContentLoaded', function() {
    let submit_button = document.getElementById('submit');

    submit_button.addEventListener('click', function(e) {
        let url = document.getElementById('url').value;

        let output_div = document.getElementById('shortened-url');

        const data = { url: url };

        fetch('http://127.0.0.1:8080/short', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(data)
        })
        .then(response => {
            if (!response.ok) {
                throw new Error('Network response was not ok');
            }
            return response.json();
        })
        .then(data => {
            output_div.innerHTML = 'Shortened URL: <a href="http://127.0.0.1:8080/short/' + data.id + '">http://127.0.0.1:8080/short/' + data.id + '</a>';
        })
        .catch(error => {
            console.error('There was a problem with the fetch operation:', error);
            output_div.innerHTML = 'Error: ' + error.message;
        });
    });
});