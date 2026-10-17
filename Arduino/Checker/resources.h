#pragma once

const char PAGE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pl">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>CHECKER</title>
  <style>
    * {
      box-sizing: border-box;
      -webkit-touch-callout: none;
      -webkit-user-select: none;
      user-select: none;
    }
    body {
      background-color: #121212;
      color: #ffffff;
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
      margin: 0;
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      touch-action: manipulation;
      padding: 20px 0;
    }

    .telemetry {
      display: flex;
      flex-direction: column;
      gap: 10px;
      width: 285px; 
      margin-bottom: 22px;
    }
    .metric-bubble {
      background: #1c1c1e;
      border: 1px solid #2c2c2e;
      border-radius: 14px;
      padding: 10px 18px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      box-shadow: 0 4px 12px rgba(0, 0, 0, 0.4);
    }
    .metric-label {
      font-size: 13px;
      text-transform: uppercase;
      letter-spacing: 0.8px;
      color: #8e8e93;
    }
    .metric-val {
      font-size: 16px;
      font-weight: 600;
      color: #ffea00;
      font-variant-numeric: tabular-nums;
    }

    h2 {
      margin: 0 0 20px 0;
      font-weight: 500;
      letter-spacing: 1.5px;
      font-size: 20px;
      color: #e5e5ea;
    }

    .pad {
      display: grid;
      grid-template-columns: repeat(3, 85px);
      grid-template-rows: repeat(3, 85px);
      gap: 15px;
    }
    .btn {
      background: #242424;
      border: 2px solid #3d3d3d;
      border-radius: 16px;
      color: #ffea00;
      font-size: 26px;
      display: flex;
      align-items: center;
      justify-content: center;
      box-shadow: 0 4px 10px rgba(0, 0, 0, 0.5);
      transition: background 0.1s, transform 0.1s, color 0.1s;
    }
    .btn:active, .btn.active {
      background: #ffea00;
      color: #121212;
      border-color: #ffea00;
      transform: scale(0.95);
    }
    .btn svg {
      stroke: #ffea00;
      transition: stroke 0.1s;
    }
    .btn:active svg, .btn.active svg {
      stroke: #121212;
    }

    .up     { grid-column: 2; grid-row: 1; }
    .left   { grid-column: 1; grid-row: 2; }
    .stop   { grid-column: 2; grid-row: 2; background: #333015; border-color: #fff830; color: #fff830; font-size: 26px; font-weight: bold; }
    .right  { grid-column: 3; grid-row: 2; }
    .horn   { grid-column: 1; grid-row: 3; }
    .down   { grid-column: 2; grid-row: 3; }
    .light  { grid-column: 3; grid-row: 3; }

    .light.is-on {
      background: #ffea00;
      border-color: #ffea00;
    }
    .light.is-on svg {
      stroke: #121212;
    }
  </style>
</head>
<body>
  <div class="telemetry">
    <div class="metric-bubble">
      <span class="metric-label">Temperatura</span>
      <span class="metric-val"><span id="temp">--</span> °C</span>
    </div>
    <div class="metric-bubble">
      <span class="metric-label">Wilgotność</span>
      <span class="metric-val"><span id="hum">--</span> %</span>
    </div>
    <div class="metric-bubble">
      <span class="metric-label">Ciśnienie</span>
      <span class="metric-val"><span id="pres">--</span> hPa</span>
    </div>
  </div>

  <h2>CHECKER 1.3</h2>

  <div class="pad">
    <div class="btn up"    data-action="B">&#9650;</div>
    <div class="btn left"  data-action="L">&#9664;</div>
    <div class="btn stop"  data-action="S">&#9679;</div>
    <div class="btn right" data-action="R">&#9654;</div>

    <!-- Klakson (lewy dół) -->
    <div class="btn horn" id="hornBtn">
      <svg viewBox="0 0 24 24" width="26" height="26" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
        <polygon points="11 5 6 9 2 9 2 15 6 15 11 19 11 5"></polygon>
        <path d="M15.54 8.46a5 5 0 0 1 0 7.07"></path>
        <path d="M19.07 4.93a10 10 0 0 1 0 14.14"></path>
      </svg>
    </div>

    <!-- W dół (środek dół) -->
    <div class="btn down"  data-action="F">&#9660;</div>

    <!-- Światła (prawy dół) -->
    <div class="btn light" id="lightBtn">
      <svg viewBox="0 0 24 24" width="28" height="28" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
        <path d="M12 2v3M4.93 4.93l2.12 2.12M2 12h3M4.93 19.07l2.12-2.12M12 22v-3"/>
        <path d="M16 8a5 5 0 0 0-7.07 7.07l1.07 1.07A2 2 0 0 0 11.41 17h1.18a2 2 0 0 0 1.41-.59l1.07-1.07A5 5 0 0 0 16 8z"/>
      </svg>
    </div>
  </div>

  <script>
    let activeCommand = 'S';

    function sendCommand(cmd) {
      if (activeCommand !== cmd) {
        activeCommand = cmd;
        fetch('/cmd?val=' + cmd, { cache: 'no-store' }).catch(() => {});
      }
    }

    // Obsługa sterowania jazdą
    const moveButtons = document.querySelectorAll('.btn[data-action]');
    moveButtons.forEach(btn => {
      const action = btn.getAttribute('data-action');
      
      btn.addEventListener('touchstart', (e) => {
        e.preventDefault();
        btn.classList.add('active');
        sendCommand(action);
      });
      btn.addEventListener('touchend', (e) => {
        e.preventDefault();
        btn.classList.remove('active');
        sendCommand('S');
      });

      btn.addEventListener('mousedown', () => {
        btn.classList.add('active');
        sendCommand(action);
      });
      btn.addEventListener('mouseup', () => {
        btn.classList.remove('active');
        sendCommand('S');
      });
    });

    // Obsługa klaksonu (trąbi przy przytrzymaniu)
    const hornBtn = document.getElementById('hornBtn');
    function startHorn(e) {
      if (e) e.preventDefault();
      hornBtn.classList.add('active');
      fetch('/cmd?val=K', { cache: 'no-store' }).catch(() => {});
    }
    function stopHorn(e) {
      if (e) e.preventDefault();
      hornBtn.classList.remove('active');
      fetch('/cmd?val=k', { cache: 'no-store' }).catch(() => {});
    }
    hornBtn.addEventListener('touchstart', startHorn);
    hornBtn.addEventListener('touchend', stopHorn);
    hornBtn.addEventListener('mousedown', startHorn);
    hornBtn.addEventListener('mouseup', stopHorn);

    // Obsługa świateł (włącz / wyłącz)
    const lightBtn = document.getElementById('lightBtn');
    function toggleLight(e) {
      if (e) e.preventDefault();
      lightBtn.classList.toggle('is-on');
      fetch('/cmd?val=H', { cache: 'no-store' }).catch(() => {});
    }
    lightBtn.addEventListener('click', toggleLight);
    lightBtn.addEventListener('touchstart', toggleLight);

    window.addEventListener('mouseup', () => {
      sendCommand('S');
      stopHorn();
    });
    window.addEventListener('touchend', () => {
      sendCommand('S');
      stopHorn();
    });

    // Pętla pobierająca telemetrię
    async function updateTelemetry() {
      try {
        const res = await fetch('/data', { cache: 'no-store' });
        if (res.ok) {
          const d = await res.json();
          document.getElementById('temp').textContent = d.temp.toFixed(1);
          document.getElementById('hum').textContent = d.hum.toFixed(1);
          document.getElementById('pres').textContent = d.pres.toFixed(2);
        }
      } catch (e) {}
    }

    setInterval(updateTelemetry, 1500);
    updateTelemetry();
  </script>
</body>
</html>
)rawliteral";