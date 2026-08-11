// script.js (улучшенный, с поддержкой новых элементов)

// === Конфигурация MQTT ===
const isSecure = window.location.protocol === 'https:';
const MQTT_HOST = window.location.hostname;
const MQTT_PORT = isSecure ? 443 : 80;
const WS_PATH = "/mqtt";

const clientId = "web_" + Math.random().toString(16).substring(2, 10);

const client = new Paho.MQTT.Client(MQTT_HOST, MQTT_PORT, WS_PATH, clientId);

// === Элементы DOM ===
const lampButton = document.getElementById('lampButton');
const lampStatus = document.getElementById('lampStatus');
const connectionStatus = document.getElementById('connectionStatus');

// === Состояние ===
let currentStatus = null;          // 'ON' или 'OFF' (null — неизвестно)
let isConnected = false;

// === Инициализация UI ===
lampButton.disabled = true;
lampStatus.innerText = 'Неизвестен';
lampStatus.className = 'value unknown';
connectionStatus.innerText = 'Соединение...';
connectionStatus.className = 'value connecting';

// === Настройки подключения ===
const connectOptions = {
    useSSL: isSecure,
    timeout: 3,
    onSuccess: () => {
        console.log('Подключено к MQTT брокеру через Nginx');
        isConnected = true;
        client.subscribe('home/lamp/status');
        lampButton.disabled = false;
        connectionStatus.innerText = 'Подключено';
        connectionStatus.className = 'value connected';
        lampStatus.innerText = 'Неизвестен (ожидание)';
        lampStatus.className = 'value unknown';
    },
    onFailure: (err) => {
        console.error('Ошибка подключения:', err);
        isConnected = false;
        lampButton.disabled = true;
        connectionStatus.innerText = 'Ошибка подключения';
        connectionStatus.className = 'value disconnected';
        lampStatus.innerText = 'Ошибка';
        lampStatus.className = 'value unknown';
    }
};

// === Обработчик потери соединения ===
client.onConnectionLost = (responseObject) => {
    console.warn('Соединение потеряно, код:', responseObject?.errorCode || 'неизвестно');
    isConnected = false;
    lampButton.disabled = true;
    connectionStatus.innerText = 'Потеря связи...';
    connectionStatus.className = 'value disconnected';
    lampStatus.innerText = 'Соединение потеряно';
    lampStatus.className = 'value unknown';

    setTimeout(() => {
        console.log('Попытка переподключения...');
        connectionStatus.innerText = 'Переподключение...';
        connectionStatus.className = 'value connecting';
        client.connect(connectOptions);
    }, 5000);
};

// === Обработчик входящих сообщений ===
client.onMessageArrived = (message) => {
    if (message.destinationName === 'home/lamp/status') {
        const status = message.payloadString; // 'ON' или 'OFF'
        currentStatus = status;

        // Обновляем статус лампы
        const statusText = status === 'ON' ? 'Включена' : 'Выключена';
        lampStatus.innerText = statusText;
        lampStatus.className = 'value ' + (status === 'ON' ? 'on' : 'off');

        // Обновляем кнопку
        const btnText = status === 'ON' ? 'Выключить' : 'Включить';
        lampButton.innerText = btnText;

        // Убираем все классы состояния кнопки и добавляем нужный
        lampButton.classList.remove('on-state', 'off-state');
        if (status === 'ON') {
            lampButton.classList.add('on-state');
        } else {
            lampButton.classList.add('off-state');
        }
    }
};

// === Обработчик клика по кнопке ===
lampButton.onclick = () => {
    if (!isConnected) {
        alert('Нет соединения с брокером. Попробуйте обновить страницу.');
        return;
    }

    if (currentStatus === null) {
        alert('Статус лампы неизвестен. Дождитесь обновления.');
        return;
    }

    const command = currentStatus === 'ON' ? 'OFF' : 'ON';
    const msg = new Paho.MQTT.Message(command);
    msg.destinationName = 'home/lamp/command';

    // Блокируем кнопку на время отправки
    lampButton.disabled = true;
    lampStatus.innerText = 'Отправка...';
    lampStatus.className = 'value connecting';

    try {
        client.send(msg);
        console.log(`Команда "${command}" отправлена`);
        // Разблокируем через 2 секунды или после получения ответа
        setTimeout(() => {
            lampButton.disabled = false;
            // Если статус не обновился, возвращаем предыдущее отображение
            if (lampStatus.innerText === 'Отправка...') {
                const prevText = currentStatus === 'ON' ? 'Включена' : 'Выключена';
                lampStatus.innerText = prevText;
                lampStatus.className = 'value ' + (currentStatus === 'ON' ? 'on' : 'off');
            }
        }, 2000);
    } catch (error) {
        console.error('Ошибка при отправке команды:', error);
        alert('Не удалось отправить команду.');
        lampButton.disabled = false;
        // Восстанавливаем предыдущий статус
        const prevText = currentStatus === 'ON' ? 'Включена' : 'Выключена';
        lampStatus.innerText = prevText;
        lampStatus.className = 'value ' + (currentStatus === 'ON' ? 'on' : 'off');
    }
};

// === Запуск подключения ===
client.connect(connectOptions);