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