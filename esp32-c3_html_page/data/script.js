// script.js
function fetchData() {
    fetch('/data')
        .then(response => response.json())
        .then(data => {
            document.getElementById('temperature').textContent = data.temperature;
            document.getElementById('humidity').textContent = data.humidity;
            document.getElementById('state').textContent = data.state;
        })
        .catch(error => console.error('Ошибка:', error));
}

function setLED(state) {
    fetch('/' + state)
        .then(response => {
            if (response.ok) {
                fetchData();
            }
        })
        .catch(error => console.error('Ошибка:', error));
}

window.onload = function() {
    fetchData();
    setInterval(fetchData, 3000);
};