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
      height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      touch-action: manipulation;
    }
    h2 {
      margin-bottom: 25px;
      font-weight: 500;
      letter-spacing: 1px;
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
      transition: background 0.1s, transform 0.1s;
    }
    .btn:active, .btn.active {
      background: #ffea00;
      color: #121212;
      border-color: #ffea00;
      transform: scale(0.95);
    }
    .up    { grid-column: 2; grid-row: 1; }
    .left  { grid-column: 1; grid-row: 2; }
    .stop  { grid-column: 2; grid-row: 2; background: #333015; border-color: #fff830; color: #fff830; font-size: 26px; font-weight: bold; }
    .right { grid-column: 3; grid-row: 2; }
    .down  { grid-column: 2; grid-row: 3; }
  </style>
</head>
<body>
  <h2>CHECKER 1.0</h2>
  <div class="pad">
    <div class="btn up"    data-action="F">&#9650;</div>
    <div class="btn left"  data-action="L">&#9664;</div>
    <div class="btn stop"  data-action="S">&#9679;</div>
    <div class="btn right" data-action="R">&#9654;</div>
    <div class="btn down"  data-action="B">&#9660;</div>
  </div>

  <script>
    let activeCommand = 'S';

    function sendCommand(cmd) {
      if (activeCommand !== cmd) {
        activeCommand = cmd;
        fetch('/cmd?val=' + cmd, { cache: 'no-store' }).catch(() => {});
      }
    }

    const buttons = document.querySelectorAll('.btn');
    buttons.forEach(btn => {
      const action = btn.getAttribute('data-action');
      
      // Zdarzenia dotykowe (smartfon)
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

      // Zdarzenia myszy (testowanie na PC)
      btn.addEventListener('mousedown', () => {
        btn.classList.add('active');
        sendCommand(action);
      });
      btn.addEventListener('mouseup', () => {
        btn.classList.remove('active');
        sendCommand('S');
      });
    });

    window.addEventListener('mouseup', () => sendCommand('S'));
    window.addEventListener('touchend', () => sendCommand('S'));
  </script>
</body>
</html>
)rawliteral";