// ESP32_4_Bulb_Web_Server_AP_Mode.ino
// FIXED FOR ESP32-S3 - SAFE GPIO PINS

#include <WiFi.h>
#include <WebServer.h>

// ========== ESP32 WiFi Hotspot Settings ==========
const char* ap_ssid = "SmartLighting_ESP32";
const char* ap_password = "12345678";

// ========== Login Credentials ==========
const char* USERNAME = "smart0010";
const char* PASSWORD = "smart0020";

// ========== SAFE GPIO PINS FOR ESP32-S3 ==========
// ✅ AVOID: GPIO 26-32 (reserved for flash/PSRAM)
// ✅ USE: GPIO 1-21, 33-48 (safe for general use)

const int RELAY_PIN_1 = 1;   // Light 1 - Living Room  ✅ SAFE
const int RELAY_PIN_2 = 2;   // Light 2 - Bedroom     ✅ SAFE
const int RELAY_PIN_3 = 3;   // Light 3 - Kitchen     ✅ SAFE
const int RELAY_PIN_4 = 4;   // Light 4 - Study Room  ✅ SAFE

// ========== Light State Variables ==========
bool lightState1 = false;
bool lightState2 = false;
bool lightState3 = false;
bool lightState4 = false;

// ========== Session Management ==========
bool isLoggedIn = false;
unsigned long loginTime = 0;
const unsigned long SESSION_TIMEOUT = 3600000;

// ========== Web Server ==========
WebServer server(80);

// ========== COMPLETE HTML ==========
const char* login_html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Lighting - Login</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #0f0c29, #302b63, #24243e);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }
        .login-container {
            background: rgba(255, 255, 255, 0.08);
            backdrop-filter: blur(20px);
            border-radius: 30px;
            padding: 50px 40px;
            max-width: 420px;
            width: 100%;
            box-shadow: 0 25px 60px rgba(0, 0, 0, 0.6);
            border: 1px solid rgba(255, 255, 255, 0.1);
        }
        .login-header { text-align: center; margin-bottom: 35px; }
        .login-header .icon { font-size: 4rem; margin-bottom: 10px; display: block; }
        .login-header h1 { color: #fff; font-size: 2rem; font-weight: 700; }
        .login-header h1 span {
            background: linear-gradient(90deg, #f7971e, #ffd200);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }
        .login-header p { color: rgba(255, 255, 255, 0.5); font-size: 0.9rem; margin-top: 8px; }
        .login-header .wifi-info {
            background: rgba(74, 222, 128, 0.1);
            border: 1px solid rgba(74, 222, 128, 0.2);
            border-radius: 10px;
            padding: 10px 16px;
            margin-top: 15px;
            font-size: 0.75rem;
            color: rgba(255, 255, 255, 0.6);
        }
        .login-header .wifi-info strong { color: #4ade80; }
        .form-group { margin-bottom: 20px; }
        .form-group label {
            display: block;
            color: rgba(255, 255, 255, 0.7);
            font-size: 0.85rem;
            margin-bottom: 8px;
            font-weight: 600;
        }
        .form-group input {
            width: 100%;
            padding: 14px 18px;
            background: rgba(255, 255, 255, 0.06);
            border: 2px solid rgba(255, 255, 255, 0.1);
            border-radius: 12px;
            color: #fff;
            font-size: 1rem;
            transition: all 0.3s;
            outline: none;
        }
        .form-group input:focus {
            border-color: #f7971e;
            background: rgba(255, 255, 255, 0.1);
        }
        .btn-login {
            width: 100%;
            padding: 16px;
            border: none;
            border-radius: 12px;
            font-size: 1.1rem;
            font-weight: 700;
            cursor: pointer;
            transition: all 0.3s;
            background: linear-gradient(135deg, #f7971e, #ffd200);
            color: #1a1a2e;
            margin-top: 10px;
        }
        .btn-login:hover { transform: scale(1.02); }
        .error-msg {
            background: rgba(239, 68, 68, 0.2);
            border: 1px solid rgba(239, 68, 68, 0.3);
            color: #fca5a5;
            padding: 12px 16px;
            border-radius: 10px;
            font-size: 0.85rem;
            margin-bottom: 20px;
            display: none;
            text-align: center;
        }
        .error-msg.show { display: block; }
        .login-footer {
            text-align: center;
            margin-top: 25px;
            color: rgba(255, 255, 255, 0.2);
            font-size: 0.75rem;
        }
        .status-dot {
            display: inline-block;
            width: 8px;
            height: 8px;
            border-radius: 50%;
            background: #4ade80;
            margin-right: 6px;
            animation: pulse 2s infinite;
        }
        @keyframes pulse {
            0% { opacity: 1; transform: scale(1); }
            50% { opacity: 0.5; transform: scale(0.8); }
            100% { opacity: 1; transform: scale(1); }
        }
    </style>
</head>
<body>
    <div class="login-container">
        <div class="login-header">
            <span class="icon">📶</span>
            <h1><span>Smart Lighting</span></h1>
            <p>Connect to ESP32 WiFi & control your lights</p>
            <div class="wifi-info">
                📡 <strong>WiFi:</strong> SmartLighting_ESP32 &nbsp;|&nbsp; 🔑 <strong>Password:</strong> 12345678
            </div>
        </div>
        <div class="error-msg" id="errorMsg">❌ Invalid username or password</div>
        <form id="loginForm" onsubmit="return handleLogin(event)">
            <div class="form-group">
                <label>👤 Username</label>
                <input type="text" id="username" placeholder="Enter username" value="smart0010" required>
            </div>
            <div class="form-group">
                <label>🔑 Password</label>
                <input type="password" id="password" placeholder="Enter password" value="smart0020" required>
            </div>
            <button type="submit" class="btn-login">🚀 Sign In</button>
        </form>
        <div class="login-footer">
            <span class="status-dot"></span> ESP32 Hotspot Active
        </div>
    </div>
    <script>
        function handleLogin(event) {
            event.preventDefault();
            const username = document.getElementById('username').value;
            const password = document.getElementById('password').value;
            const errorMsg = document.getElementById('errorMsg');
            fetch('/login', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: 'username=' + encodeURIComponent(username) + '&password=' + encodeURIComponent(password)
            })
            .then(response => response.text())
            .then(data => {
                if (data === 'OK') { window.location.href = '/dashboard'; }
                else {
                    errorMsg.textContent = '❌ Invalid username or password';
                    errorMsg.classList.add('show');
                    setTimeout(() => errorMsg.classList.remove('show'), 3000);
                }
            })
            .catch(err => {
                errorMsg.textContent = '❌ Connection error';
                errorMsg.classList.add('show');
            });
            return false;
        }
    </script>
</body>
</html>
)rawliteral";

const char* dashboard_html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Lighting Control</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #0f0c29, #302b63, #24243e);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }
        .container {
            background: rgba(255, 255, 255, 0.08);
            backdrop-filter: blur(20px);
            border-radius: 30px;
            padding: 40px;
            max-width: 900px;
            width: 100%;
            box-shadow: 0 25px 60px rgba(0, 0, 0, 0.6);
            border: 1px solid rgba(255, 255, 255, 0.1);
        }
        .header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 30px;
            flex-wrap: wrap;
            gap: 15px;
        }
        .header-left h1 { color: #fff; font-size: 2rem; font-weight: 700; }
        .header-left h1 span {
            background: linear-gradient(90deg, #f7971e, #ffd200);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }
        .header-left .ap-info { color: rgba(255, 255, 255, 0.4); font-size: 0.75rem; margin-top: 4px; }
        .header-left .ap-info strong { color: #4ade80; }
        .header-right { display: flex; align-items: center; gap: 15px; }
        .status-badge {
            display: flex;
            align-items: center;
            gap: 8px;
            padding: 8px 16px;
            background: rgba(255, 255, 255, 0.06);
            border-radius: 20px;
            border: 1px solid rgba(255, 255, 255, 0.08);
        }
        .status-dot {
            width: 10px;
            height: 10px;
            border-radius: 50%;
            background: #4ade80;
            animation: pulse 2s infinite;
        }
        @keyframes pulse {
            0% { opacity: 1; transform: scale(1); }
            50% { opacity: 0.5; transform: scale(0.8); }
            100% { opacity: 1; transform: scale(1); }
        }
        .status-text { color: rgba(255, 255, 255, 0.7); font-size: 0.8rem; }
        .btn-logout {
            padding: 8px 20px;
            background: rgba(239, 68, 68, 0.2);
            border: 1px solid rgba(239, 68, 68, 0.3);
            border-radius: 20px;
            color: #fca5a5;
            font-size: 0.8rem;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s;
        }
        .btn-logout:hover { background: rgba(239, 68, 68, 0.3); }
        .grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
            gap: 20px;
            margin-bottom: 25px;
        }
        .light-card {
            background: rgba(255, 255, 255, 0.06);
            border-radius: 20px;
            padding: 25px 15px;
            text-align: center;
            cursor: pointer;
            transition: all 0.4s;
            border: 2px solid transparent;
            user-select: none;
            position: relative;
        }
        .light-card:hover { transform: translateY(-5px); background: rgba(255, 255, 255, 0.1); }
        .light-card:active { transform: scale(0.95); }
        .light-card.on {
            border-color: #fbbf24;
            background: rgba(251, 191, 36, 0.15);
            box-shadow: 0 0 40px rgba(251, 191, 36, 0.15);
        }
        .light-card.off { border-color: rgba(255, 255, 255, 0.08); opacity: 0.7; }
        .light-icon { font-size: 3.5rem; margin-bottom: 12px; }
        .light-card.on .light-icon {
            animation: glow 1.5s ease-in-out infinite alternate;
        }
        @keyframes glow {
            0% { filter: drop-shadow(0 0 5px rgba(251, 191, 36, 0.3)); }
            100% { filter: drop-shadow(0 0 25px rgba(251, 191, 36, 0.8)); }
        }
        .light-number {
            position: absolute;
            top: 10px;
            right: 12px;
            color: rgba(255, 255, 255, 0.15);
            font-size: 0.7rem;
            font-weight: 700;
        }
        .light-name { color: #fff; font-size: 1.1rem; font-weight: 600; margin-bottom: 4px; }
        .light-room { color: rgba(255, 255, 255, 0.4); font-size: 0.8rem; margin-bottom: 12px; }
        .light-status {
            display: inline-block;
            padding: 4px 18px;
            border-radius: 20px;
            font-size: 0.75rem;
            font-weight: 700;
        }
        .light-status.on { background: #4ade80; color: #0f0c29; }
        .light-status.off { background: #ef4444; color: #fff; }
        .controls { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; }
        .btn {
            padding: 16px;
            border: none;
            border-radius: 15px;
            font-size: 1rem;
            font-weight: 700;
            cursor: pointer;
            transition: all 0.3s;
        }
        .btn:active { transform: scale(0.97); }
        .btn-all-on {
            background: linear-gradient(135deg, #f7971e, #ffd200);
            color: #1a1a2e;
        }
        .btn-all-on:hover { transform: scale(1.02); }
        .btn-all-off {
            background: rgba(255, 255, 255, 0.08);
            color: #fff;
            border: 2px solid rgba(255, 255, 255, 0.15);
        }
        .btn-all-off:hover { background: rgba(255, 255, 255, 0.15); transform: scale(1.02); }
        .footer {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-top: 25px;
            color: rgba(255, 255, 255, 0.2);
            font-size: 0.75rem;
            flex-wrap: wrap;
            gap: 10px;
        }
        .footer .user-info { color: rgba(255, 255, 255, 0.3); }
        .toast {
            position: fixed;
            bottom: 30px;
            left: 50%;
            transform: translateX(-50%) translateY(100px);
            background: rgba(0, 0, 0, 0.85);
            color: #fff;
            padding: 12px 30px;
            border-radius: 12px;
            font-size: 0.9rem;
            transition: all 0.5s cubic-bezier(0.68, -0.55, 0.265, 1.55);
            opacity: 0;
            backdrop-filter: blur(10px);
            border: 1px solid rgba(255, 255, 255, 0.1);
            z-index: 1000;
        }
        .toast.show { opacity: 1; transform: translateX(-50%) translateY(0); }
        .toast.success { border-color: rgba(74, 222, 128, 0.3); }
        .toast.error { border-color: rgba(239, 68, 68, 0.3); }
        @media (max-width: 600px) {
            .container { padding: 20px; }
            .header { flex-direction: column; align-items: flex-start; }
            .header-left h1 { font-size: 1.6rem; }
            .header-right { width: 100%; justify-content: space-between; }
            .grid { grid-template-columns: 1fr 1fr; gap: 12px; }
            .light-card { padding: 18px 10px; }
            .light-icon { font-size: 2.5rem; }
            .controls { grid-template-columns: 1fr; }
        }
        @media (max-width: 400px) { .grid { grid-template-columns: 1fr; } }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <div class="header-left">
                <h1>💡 <span>Smart Lighting</span></h1>
                <p class="ap-info">📶 Connected to <strong>SmartLighting_ESP32</strong></p>
            </div>
            <div class="header-right">
                <div class="status-badge">
                    <div class="status-dot"></div>
                    <span class="status-text">Online</span>
                </div>
                <button class="btn-logout" onclick="handleLogout()">🚪 Logout</button>
            </div>
        </div>
        <div class="grid" id="lightGrid">
            <div class="light-card off" id="card1" onclick="toggleLight(1)">
                <span class="light-number">#1</span>
                <div class="light-icon">💡</div>
                <div class="light-name">Light 1</div>
                <div class="light-room">Living Room</div>
                <span class="light-status off" id="status1">OFF</span>
            </div>
            <div class="light-card off" id="card2" onclick="toggleLight(2)">
                <span class="light-number">#2</span>
                <div class="light-icon">💡</div>
                <div class="light-name">Light 2</div>
                <div class="light-room">Bedroom</div>
                <span class="light-status off" id="status2">OFF</span>
            </div>
            <div class="light-card off" id="card3" onclick="toggleLight(3)">
                <span class="light-number">#3</span>
                <div class="light-icon">💡</div>
                <div class="light-name">Light 3</div>
                <div class="light-room">Kitchen</div>
                <span class="light-status off" id="status3">OFF</span>
            </div>
            <div class="light-card off" id="card4" onclick="toggleLight(4)">
                <span class="light-number">#4</span>
                <div class="light-icon">💡</div>
                <div class="light-name">Light 4</div>
                <div class="light-room">Study Room</div>
                <span class="light-status off" id="status4">OFF</span>
            </div>
        </div>
        <div class="controls">
            <button class="btn btn-all-on" onclick="controlAll('ON')">🔆 ALL ON</button>
            <button class="btn btn-all-off" onclick="controlAll('OFF')">🌙 ALL OFF</button>
        </div>
        <div class="footer">
            <span>📶 ESP32 Hotspot • 192.168.4.1</span>
            <span class="user-info">👤 smart0010</span>
        </div>
    </div>
    <div class="toast" id="toast"></div>
    <script>
        function showToast(message, type = 'success') {
            const toast = document.getElementById('toast');
            toast.textContent = message;
            toast.className = 'toast ' + type;
            setTimeout(() => toast.classList.add('show'), 10);
            setTimeout(() => toast.classList.remove('show'), 2500);
        }
        function toggleLight(id) {
            const card = document.getElementById('card' + id);
            const status = document.getElementById('status' + id);
            const isOn = card.classList.contains('on');
            fetch('/toggle?id=' + id)
                .then(response => response.text())
                .then(data => {
                    if (data === 'OK') {
                        const newState = !isOn;
                        updateUI(id, newState);
                        showToast('Light ' + id + ' ' + (newState ? 'ON' : 'OFF'), newState ? 'success' : 'error');
                    } else if (data === 'SESSION_EXPIRED') {
                        window.location.href = '/';
                    }
                })
                .catch(err => { showToast('Connection error', 'error'); });
        }
        function controlAll(state) {
            fetch('/all?state=' + state)
                .then(response => response.text())
                .then(data => {
                    if (data === 'OK') {
                        for (let i = 1; i <= 4; i++) {
                            const newState = (state === 'ON');
                            updateUI(i, newState);
                        }
                        showToast('All lights ' + state, state === 'ON' ? 'success' : 'error');
                    } else if (data === 'SESSION_EXPIRED') {
                        window.location.href = '/';
                    }
                })
                .catch(err => { showToast('Connection error', 'error'); });
        }
        function updateUI(id, isOn) {
            const card = document.getElementById('card' + id);
            const status = document.getElementById('status' + id);
            if (isOn) {
                card.classList.remove('off');
                card.classList.add('on');
                status.textContent = 'ON';
                status.className = 'light-status on';
            } else {
                card.classList.remove('on');
                card.classList.add('off');
                status.textContent = 'OFF';
                status.className = 'light-status off';
            }
        }
        function handleLogout() {
            fetch('/logout')
                .then(() => { window.location.href = '/'; });
        }
        function fetchStates() {
            fetch('/states')
                .then(response => response.json())
                .then(data => {
                    if (data.authenticated === false) {
                        window.location.href = '/';
                        return;
                    }
                    for (let i = 1; i <= 4; i++) {
                        updateUI(i, data['light' + i]);
                    }
                })
                .catch(err => {});
        }
        window.onload = function() {
            fetchStates();
            setInterval(fetchStates, 30000);
        };
    </script>
</body>
</html>
)rawliteral";

// ========== SETUP ==========
void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n\n=== COMPLETE SMART LIGHTING SYSTEM ===");
  Serial.println("✅ Using SAFE GPIO pins for ESP32-S3");
  Serial.println("📶 Creating WiFi Hotspot...");
  
  // Initialize relay pins (SAFE pins for ESP32-S3)
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(RELAY_PIN_3, OUTPUT);
  pinMode(RELAY_PIN_4, OUTPUT);
  digitalWrite(RELAY_PIN_1, LOW);
  digitalWrite(RELAY_PIN_2, LOW);
  digitalWrite(RELAY_PIN_3, LOW);
  digitalWrite(RELAY_PIN_4, LOW);
  
  Serial.println("✅ Relay pins initialized on GPIO 1,2,3,4");
  
  // WiFi
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  Serial.print("✅ WiFi AP Started! IP: ");
  Serial.println(WiFi.softAPIP());
  
  // WebServer routes
  server.on("/", []() {
    server.send(200, "text/html", login_html);
  });
  
  server.on("/dashboard", []() {
    if (!isLoggedIn) {
      server.sendHeader("Location", "/", true);
      server.send(302, "text/plain", "");
      return;
    }
    server.send(200, "text/html", dashboard_html);
  });
  
  server.on("/login", HTTP_POST, []() {
    if (server.hasArg("username") && server.hasArg("password")) {
      if (server.arg("username") == USERNAME && server.arg("password") == PASSWORD) {
        isLoggedIn = true;
        loginTime = millis();
        server.send(200, "text/plain", "OK");
        return;
      }
    }
    server.send(401, "text/plain", "Invalid");
  });
  
  server.on("/logout", []() {
    isLoggedIn = false;
    server.send(200, "text/plain", "OK");
  });
  
  server.on("/toggle", []() {
    if (!isLoggedIn) { server.send(401, "text/plain", "SESSION_EXPIRED"); return; }
    if (server.hasArg("id")) {
      int id = server.arg("id").toInt();
      switch(id) {
        case 1: lightState1 = !lightState1; digitalWrite(RELAY_PIN_1, lightState1 ? HIGH : LOW); break;
        case 2: lightState2 = !lightState2; digitalWrite(RELAY_PIN_2, lightState2 ? HIGH : LOW); break;
        case 3: lightState3 = !lightState3; digitalWrite(RELAY_PIN_3, lightState3 ? HIGH : LOW); break;
        case 4: lightState4 = !lightState4; digitalWrite(RELAY_PIN_4, lightState4 ? HIGH : LOW); break;
      }
      server.send(200, "text/plain", "OK");
      return;
    }
    server.send(400, "text/plain", "Invalid");
  });
  
  server.on("/all", []() {
    if (!isLoggedIn) { server.send(401, "text/plain", "SESSION_EXPIRED"); return; }
    if (server.hasArg("state")) {
      bool turnOn = (server.arg("state") == "ON");
      lightState1 = turnOn; lightState2 = turnOn; lightState3 = turnOn; lightState4 = turnOn;
      digitalWrite(RELAY_PIN_1, turnOn ? HIGH : LOW);
      digitalWrite(RELAY_PIN_2, turnOn ? HIGH : LOW);
      digitalWrite(RELAY_PIN_3, turnOn ? HIGH : LOW);
      digitalWrite(RELAY_PIN_4, turnOn ? HIGH : LOW);
      server.send(200, "text/plain", "OK");
      return;
    }
    server.send(400, "text/plain", "Invalid");
  });
  
  server.on("/states", []() {
    if (!isLoggedIn) {
      server.send(200, "application/json", "{\"authenticated\":false}");
      return;
    }
    String json = "{";
    json += "\"authenticated\":true,";
    json += "\"light1\":" + String(lightState1 ? "true" : "false") + ",";
    json += "\"light2\":" + String(lightState2 ? "true" : "false") + ",";
    json += "\"light3\":" + String(lightState3 ? "true" : "false") + ",";
    json += "\"light4\":" + String(lightState4 ? "true" : "false");
    json += "}";
    server.send(200, "application/json", json);
  });

  server.begin();
  Serial.println("✅ WebServer Started!");
  Serial.println("========================================");
  Serial.println("📱 Connect to WiFi: " + String(ap_ssid));
  Serial.println("🔑 Password: " + String(ap_password));
  Serial.println("🌐 Open browser: http://192.168.4.1");
  Serial.println("🔑 Login: " + String(USERNAME) + " / " + String(PASSWORD));
  Serial.println("========================================");
  Serial.println("🔌 PIN MAP FOR RELAYS:");
  Serial.println("   Light 1 (Living Room)  → GPIO 1");
  Serial.println("   Light 2 (Bedroom)      → GPIO 2");
  Serial.println("   Light 3 (Kitchen)      → GPIO 3");
  Serial.println("   Light 4 (Study Room)   → GPIO 4");
  Serial.println("========================================");
}

// ========== LOOP ==========
void loop() {
  server.handleClient();
  if (isLoggedIn && (millis() - loginTime > SESSION_TIMEOUT)) {
    isLoggedIn = false;
  }
}