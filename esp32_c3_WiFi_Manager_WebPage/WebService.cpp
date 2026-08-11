#include "WebService.h"

// ======== HTML-страницы (PROGMEM) ========

// Страница для режима AP (настройка)
static const char AP_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Настройка ESP32</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .container { max-width: 600px; margin: auto; }
        input, button { padding: 8px; margin: 5px 0; width: 100%; box-sizing: border-box; }
        .network { background: #f0f0f0; padding: 10px; margin: 5px 0; border-radius: 4px; }
        .network span { display: inline-block; width: 60%; }
        .network button { width: auto; }
        .saved-network { background: #d0e0d0; padding: 10px; margin: 5px 0; border-radius: 4px; }
        .saved-network span { display: inline-block; width: 60%; }
        .saved-network button { width: auto; }
        .mqtt-settings { border: 1px solid #ccc; padding: 15px; margin: 15px 0; border-radius: 4px; }
        .mqtt-settings label { display: block; margin-top: 5px; }
        .hidden { display: none; }
        .error { color: red; }
        .success { color: green; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Настройка ESP32</h1>
        <div id="status"></div>
        <button id="scanBtn">Сканировать сети</button>
        <div id="networks"></div>
        <h3>Сохранённые сети</h3>
        <div id="savedNetworks"></div>
        <div class="mqtt-settings">
            <h3>Настройки MQTT</h3>
            <label>Сервер: <input id="mqttServer" placeholder="192.168.0.100"></label>
            <label>Порт: <input id="mqttPort" placeholder="1883" value="1883"></label>
            <label>Логин: <input id="mqttUser" placeholder="user"></label>
            <label>Пароль: <input id="mqttPass" placeholder="password" type="password"></label>
            <label>Командный топик: <input id="mqttCmd" placeholder="home/lamp/command"></label>
            <label>Топик состояния: <input id="mqttState" placeholder="home/lamp/status"></label>
            <button id="saveMqttBtn">Сохранить MQTT</button>
        </div>
        <button id="saveRebootBtn">Сохранить и перезагрузить</button>
    </div>
    <script>
        const statusDiv = document.getElementById('status');
        const networksDiv = document.getElementById('networks');
        const savedDiv = document.getElementById('savedNetworks');
        const scanBtn = document.getElementById('scanBtn');
        const saveRebootBtn = document.getElementById('saveRebootBtn');
        const saveMqttBtn = document.getElementById('saveMqttBtn');

        function showStatus(msg, isError) {
            statusDiv.innerHTML = '<div class="' + (isError ? 'error' : 'success') + '">' + msg + '</div>';
        }

        function loadSavedNetworks() {
            fetch('/saved')
                .then(res => res.json())
                .then(data => {
                    savedDiv.innerHTML = '';
                    if (data.length === 0) {
                        savedDiv.innerHTML = '<p>Нет сохранённых сетей.</p>';
                        return;
                    }
                    data.forEach((ssid, idx) => {
                        const div = document.createElement('div');
                        div.className = 'saved-network';
                        div.innerHTML = '<span>' + ssid + '</span> <button onclick="deleteNetwork(' + idx + ')">Удалить</button>';
                        savedDiv.appendChild(div);
                    });
                });
        }

        function loadMqttSettings() {
            fetch('/mqtt')
                .then(res => res.json())
                .then(data => {
                    document.getElementById('mqttServer').value = data.server || '';
                    document.getElementById('mqttPort').value = data.port || '1883';
                    document.getElementById('mqttUser').value = data.user || '';
                    document.getElementById('mqttPass').value = data.password || '';
                    document.getElementById('mqttCmd').value = data.cmdTopic || '';
                    document.getElementById('mqttState').value = data.stateTopic || '';
                });
        }

        scanBtn.onclick = function() {
            showStatus('Сканирование...', false);
            fetch('/scan')
                .then(res => {
                    if (!res.ok) throw new Error('HTTP error ' + res.status);
                    return res.json();
                })
                .then(data => {
                    networksDiv.innerHTML = '';
                    if (data.length === 0) {
                        networksDiv.innerHTML = '<p>Сетей не найдено.</p>';
                        return;
                    }
                    data.forEach((net) => {
                        const div = document.createElement('div');
                        div.className = 'network';
                        const secured = net.encryption !== 'OPEN' ? '🔒' : '🔓';
                        div.innerHTML = '<span>' + net.ssid + ' ' + secured + ' (RSSI: ' + net.rssi + ')</span> <button onclick="connectTo(\'' + net.ssid + '\')">Подключиться</button>';
                        networksDiv.appendChild(div);
                    });
                    showStatus('Найдено ' + data.length + ' сетей', false);
                })
                .catch(error => {
                    console.error('Ошибка сканирования:', error);
                    showStatus('Не удалось выполнить сканирование. Проверьте соединение с ESP32 и нажмите "Сканировать сети" снова.', true);
                });
        };

        function connectTo(ssid) {
            const pass = prompt('Введите пароль для сети "' + ssid + '":');
            if (pass === null) return;
            fetch('/connect', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ ssid: ssid, password: pass })
            })
            .then(res => res.json())
            .then(data => {
                if (data.success) {
                    showStatus('Подключено к ' + ssid + ', сохранено. Перезагрузка...', false);
                    setTimeout(() => { location.reload(); }, 3000);
                } else {
                    showStatus('Ошибка подключения: ' + data.error, true);
                }
            });
        }

        function deleteNetwork(index) {
            if (!confirm('Удалить сеть?')) return;
            fetch('/delete', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ index: index })
            })
            .then(res => res.json())
            .then(data => {
                if (data.success) {
                    showStatus('Сеть удалена', false);
                    loadSavedNetworks();
                } else {
                    showStatus('Ошибка удаления', true);
                }
            });
        }

        saveMqttBtn.onclick = function() {
            const data = {
                server: document.getElementById('mqttServer').value,
                port: parseInt(document.getElementById('mqttPort').value) || 1883,
                user: document.getElementById('mqttUser').value,
                password: document.getElementById('mqttPass').value,
                cmdTopic: document.getElementById('mqttCmd').value,
                stateTopic: document.getElementById('mqttState').value
            };
            fetch('/mqtt', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(data)
            })
            .then(res => res.json())
            .then(data => {
                if (data.success) {
                    showStatus('Настройки MQTT сохранены', false);
                } else {
                    showStatus('Ошибка сохранения MQTT', true);
                }
            });
        };

        saveRebootBtn.onclick = function() {
            if (confirm('Сохранить настройки и перезагрузить?')) {
                saveMqttBtn.click();
                fetch('/reboot', { method: 'POST' })
                .then(res => res.json())
                .then(data => {
                    showStatus('Перезагрузка...', false);
                    setTimeout(() => {}, 2000);
                });
            }
        };

        // Инициализация
        loadSavedNetworks();
        loadMqttSettings();
        // Автоматическое сканирование через 500 мс
        setTimeout(function() {
            scanBtn.click();
        }, 500);
    </script>
</body>
</html>
)rawliteral";

// Страница для клиентского режима (управление)
static const char CLIENT_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Управление ESP32</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .container { max-width: 600px; margin: auto; }
        .status { background: #f0f0f0; padding: 15px; border-radius: 4px; }
        .led-control { margin: 20px 0; }
        button { padding: 10px 20px; margin: 5px; }
        .mqtt-settings { border: 1px solid #ccc; padding: 15px; margin: 15px 0; border-radius: 4px; }
        .mqtt-settings label { display: block; margin-top: 5px; }
        input { padding: 8px; margin: 5px 0; width: 100%; box-sizing: border-box; }
        .hidden { display: none; }
        .error { color: red; }
        .success { color: green; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Управление ESP32</h1>
        <div id="statusDiv" class="status">Загрузка...</div>
        <div class="led-control">
            <h3>Светодиод</h3>
            <button id="ledOnBtn">Включить</button>
            <button id="ledOffBtn">Выключить</button>
            <span id="ledState">Неизвестно</span>
        </div>
        <div class="mqtt-settings">
            <h3>Настройки MQTT</h3>
            <label>Сервер: <input id="mqttServer" placeholder="192.168.0.100"></label>
            <label>Порт: <input id="mqttPort" placeholder="1883" value="1883"></label>
            <label>Логин: <input id="mqttUser" placeholder="user"></label>
            <label>Пароль: <input id="mqttPass" placeholder="password" type="password"></label>
            <label>Командный топик: <input id="mqttCmd" placeholder="home/lamp/command"></label>
            <label>Топик состояния: <input id="mqttState" placeholder="home/lamp/status"></label>
            <button id="saveMqttBtn">Сохранить MQTT</button>
        </div>
        <button id="resetBtn">Сбросить настройки и перейти в режим AP</button>
        <button id="rebootBtn">Перезагрузить</button>
    </div>
    <script>
        const statusDiv = document.getElementById('statusDiv');
        const ledStateSpan = document.getElementById('ledState');
        const ledOnBtn = document.getElementById('ledOnBtn');
        const ledOffBtn = document.getElementById('ledOffBtn');
        const saveMqttBtn = document.getElementById('saveMqttBtn');
        const resetBtn = document.getElementById('resetBtn');
        const rebootBtn = document.getElementById('rebootBtn');

        function updateStatus() {
            fetch('/status')
                .then(res => res.json())
                .then(data => {
                    let statusText = 'Подключено: ' + (data.connected ? 'Да' : 'Нет');
                    if (data.connected) {
                        statusText += ', SSID: ' + data.ssid + ', IP: ' + data.ip + ', RSSI: ' + data.rssi;
                    }
                    statusText += ', MQTT: ' + (data.mqtt ? 'Подключено' : 'Отключено');
                    statusDiv.innerHTML = statusText;
                    ledStateSpan.textContent = data.led ? 'Включён' : 'Выключен';
                });
        }

        function updateMqttSettings() {
            fetch('/mqtt')
                .then(res => res.json())
                .then(data => {
                    document.getElementById('mqttServer').value = data.server || '';
                    document.getElementById('mqttPort').value = data.port || '1883';
                    document.getElementById('mqttUser').value = data.user || '';
                    document.getElementById('mqttPass').value = data.password || '';
                    document.getElementById('mqttCmd').value = data.cmdTopic || '';
                    document.getElementById('mqttState').value = data.stateTopic || '';
                });
        }

        ledOnBtn.onclick = function() {
            fetch('/led', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ state: 'ON' })
            })
            .then(res => res.json())
            .then(data => {
                if (data.success) updateStatus();
            });
        };

        ledOffBtn.onclick = function() {
            fetch('/led', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ state: 'OFF' })
            })
            .then(res => res.json())
            .then(data => {
                if (data.success) updateStatus();
            });
        };

        saveMqttBtn.onclick = function() {
            const data = {
                server: document.getElementById('mqttServer').value,
                port: parseInt(document.getElementById('mqttPort').value) || 1883,
                user: document.getElementById('mqttUser').value,
                password: document.getElementById('mqttPass').value,
                cmdTopic: document.getElementById('mqttCmd').value,
                stateTopic: document.getElementById('mqttState').value
            };
            fetch('/mqtt', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(data)
            })
            .then(res => res.json())
            .then(data => {
                if (data.success) {
                    alert('Настройки MQTT сохранены');
                } else {
                    alert('Ошибка сохранения MQTT');
                }
            });
        };

        resetBtn.onclick = function() {
            if (confirm('Удалить все сохранённые сети и перейти в режим AP?')) {
                fetch('/reset', { method: 'POST' })
                .then(res => res.json())
                .then(data => {
                    alert('Сброс выполнен, перезагрузка...');
                    setTimeout(() => { location.reload(); }, 2000);
                });
            }
        };

        rebootBtn.onclick = function() {
            if (confirm('Перезагрузить устройство?')) {
                fetch('/reboot', { method: 'POST' })
                .then(res => res.json())
                .then(data => {
                    alert('Перезагрузка...');
                });
            }
        };

        // Обновляем статус каждые 5 секунд
        updateStatus();
        updateMqttSettings();
        setInterval(updateStatus, 5000);
    </script>
</body>
</html>
)rawliteral";


WebService::WebService(AppState& appState)
    : _appState(appState), _server(80), _apMode(false)
{
    setupRoutes();
}

WebService::~WebService() {
    _server.stop();
}


void WebService::begin(bool apMode) {
    _apMode = apMode;
    if (apMode) {
        // Включаем режим AP+STA, чтобы клиент не терял соединение при сканировании
        WiFi.mode(WIFI_AP_STA);
        String ssid = "ESP32-Setup";
        String password = "12345678";
        WiFi.softAP(ssid.c_str(), password.c_str());
        _appState.getWiFiService().setApCredentials(ssid, password);
        Serial.println("AP запущен, IP: 192.168.4.1");
    } else {
        // В клиентском режиме используем только STA
        WiFi.mode(WIFI_STA);
        Serial.print("Веб-сервер запущен, IP: ");
        Serial.println(WiFi.localIP());
    }
    _server.begin();
}

void WebService::handleClient() {
    _server.handleClient();
}

void WebService::setupRoutes() {
    _server.on("/", HTTP_GET, std::bind(&WebService::handleRoot, this));
    _server.on("/scan", HTTP_GET, std::bind(&WebService::handleScan, this));
    _server.on("/connect", HTTP_POST, std::bind(&WebService::handleConnect, this));
    _server.on("/saved", HTTP_GET, std::bind(&WebService::handleSaved, this));
    _server.on("/delete", HTTP_POST, std::bind(&WebService::handleDelete, this));
    _server.on("/status", HTTP_GET, std::bind(&WebService::handleStatus, this));
    _server.on("/led", HTTP_POST, std::bind(&WebService::handleLed, this));
    _server.on("/mqtt", HTTP_GET, std::bind(&WebService::handleMqttGet, this));
    _server.on("/mqtt", HTTP_POST, std::bind(&WebService::handleMqttPost, this));
    _server.on("/reset", HTTP_POST, std::bind(&WebService::handleReset, this));
    _server.on("/reboot", HTTP_POST, std::bind(&WebService::handleReboot, this));
    _server.onNotFound(std::bind(&WebService::handleNotFound, this));
}

void WebService::handleRoot() {
    if (_apMode) {
        _server.send_P(200, "text/html", AP_PAGE);
    } else {
        _server.send_P(200, "text/html", CLIENT_PAGE);
    }
}

void WebService::handleScan() {
    Serial.println("🔍 GET /scan");
    _appState.startScan();
    delay(100);
    String json = getScanJson();
    _server.send(200, "application/json", json);
}

void WebService::handleConnect() {
    if (!_server.hasArg("plain")) {
        _server.send(400, "application/json", "{\"success\":false,\"error\":\"No data\"}");
        return;
    }
    String body = _server.arg("plain");
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, body);
    if (error) {
        _server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid JSON\"}");
        return;
    }
    String ssid = doc["ssid"].as<String>();
    String password = doc["password"].as<String>();
    if (ssid.length() == 0) {
        _server.send(400, "application/json", "{\"success\":false,\"error\":\"SSID empty\"}");
        return;
    }
    bool success = _appState.connectToNetwork(ssid, password);
    if (success) {
        _appState.saveCurrentNetwork();
        _server.send(200, "application/json", "{\"success\":true}");
    } else {
        _server.send(500, "application/json", "{\"success\":false,\"error\":\"Connection failed\"}");
    }
}

void WebService::handleSaved() {
    String json = getSavedJson();
    _server.send(200, "application/json", json);
}

void WebService::handleDelete() {
    if (!_server.hasArg("plain")) {
        _server.send(400, "application/json", "{\"success\":false,\"error\":\"No data\"}");
        return;
    }
    String body = _server.arg("plain");
    DynamicJsonDocument doc(128);
    DeserializationError error = deserializeJson(doc, body);
    if (error) {
        _server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid JSON\"}");
        return;
    }
    int index = doc["index"].as<int>();
    bool success = _appState.deleteSavedNetwork(index);
    _server.send(200, "application/json", "{\"success\":" + String(success ? "true" : "false") + "}");
}

void WebService::handleStatus() {
    String json = _appState.getStatusJson();
    _server.send(200, "application/json", json);
}

void WebService::handleLed() {
    if (!_server.hasArg("plain")) {
        _server.send(400, "application/json", "{\"success\":false,\"error\":\"No data\"}");
        return;
    }
    String body = _server.arg("plain");
    DynamicJsonDocument doc(128);
    DeserializationError error = deserializeJson(doc, body);
    if (error) {
        _server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid JSON\"}");
        return;
    }
    String state = doc["state"].as<String>();
    if (state == "ON") {
        _appState.setLed(true);
    } else if (state == "OFF") {
        _appState.setLed(false);
    } else {
        _server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid state\"}");
        return;
    }
    _server.send(200, "application/json", "{\"success\":true}");
}

void WebService::handleMqttGet() {
    String json = getMqttSettingsJson();
    _server.send(200, "application/json", json);
}

void WebService::handleMqttPost() {
    if (!_server.hasArg("plain")) {
        _server.send(400, "application/json", "{\"success\":false,\"error\":\"No data\"}");
        return;
    }
    String body = _server.arg("plain");
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, body);
    if (error) {
        _server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid JSON\"}");
        return;
    }
    String server = doc["server"].as<String>();
    int port = doc["port"].as<int>();
    String user = doc["user"].as<String>();
    String password = doc["password"].as<String>();
    String cmdTopic = doc["cmdTopic"].as<String>();
    String stateTopic = doc["stateTopic"].as<String>();

    bool success = _appState.configureMqtt(server, port, user, password, cmdTopic, stateTopic);
    if (success) {
        _appState.beginMqtt();
        _server.send(200, "application/json", "{\"success\":true}");
    } else {
        _server.send(500, "application/json", "{\"success\":false,\"error\":\"Save failed\"}");
    }
}

void WebService::handleReset() {
    while (_appState.getSavedCount() > 0) {
        _appState.deleteSavedNetwork(0);
    }
    _appState.configureMqtt("", 1883, "", "", "", "");
    _server.send(200, "application/json", "{\"success\":true}");
    delay(100);
    ESP.restart();
}

void WebService::handleReboot() {
    _server.send(200, "application/json", "{\"success\":true}");
    delay(100);
    ESP.restart();
}

void WebService::handleNotFound() {
    _server.send(404, "text/plain", "Not found");
}

// --- Вспомогательные методы (без изменений) ---
String WebService::getScanJson() {
    int count = _appState.getNetworkCount();
    String json = "[";
    for (int i = 0; i < count; i++) {
        NetworkInfo net = _appState.getNetwork(i);
        if (i > 0) json += ",";
        json += "{";
        json += "\"ssid\":\"" + String(net.ssid) + "\",";
        json += "\"rssi\":" + String(net.rssi) + ",";
        json += "\"encryption\":\"" + String(_appState.getEncryptionType(net.encryptionType)) + "\",";
        json += "\"channel\":" + String(net.channel) + ",";
        json += "\"hidden\":" + String(net.hidden ? "true" : "false");
        json += "}";
    }
    json += "]";
    return json;
}

String WebService::getSavedJson() {
    int count = _appState.getSavedCount();
    String json = "[";
    for (int i = 0; i < count; i++) {
        if (i > 0) json += ",";
        json += "\"" + _appState.getSavedSSID(i) + "\"";
    }
    json += "]";
    return json;
}

String WebService::getMqttSettingsJson() {
    MqttCredentials& creds = _appState.getMqttCredentials();
    String json = "{";
    json += "\"server\":\"" + creds.getServer() + "\",";
    json += "\"port\":" + String(creds.getPort()) + ",";
    json += "\"user\":\"" + creds.getUser() + "\",";
    json += "\"password\":\"" + creds.getPassword() + "\",";
    json += "\"cmdTopic\":\"" + creds.getCmdTopic() + "\",";
    json += "\"stateTopic\":\"" + creds.getStateTopic() + "\"";
    json += "}";
    return json;
}