function updateLightStatus(lightId, value) {
    document.getElementById(`slider_${lightId}`).value = value;
    const indicator = document.getElementById(`status_${lightId}`);
    const intensity = Math.round((value / 255) * 100);

    // Reset classes
    indicator.className = "status-indicator";

    if (value > 0) {
      indicator.classList.add("on", `led${lightId.slice(-1)}`);

      // Efecto de brillo proporcional a la intensidad
      indicator.style.opacity = 0.3 + (intensity / 100) * 0.7;

      if (value < 255) {
        indicator.classList.add("dim");
      }
    } else {
      indicator.classList.add("off");
    }
  }
  function updateServoStatus(device, value) {
    const indicator = document.getElementById(`motor-status`);
    const state = document.getElementById(`motor-state`);
    if (device === "motor_up") {
      indicator.innerHTML =
        "Persiana Subiendo <span class='arrow-up'>↑</span>";
      state.innerHTML = "<span class='arrow-up'>↑</span>";
      setTimeout(() => {
        indicator.innerHTML = "Persiana abierta";
        state.innerHTML = "<span class='window-open'>🌄</span>";
      }, 1000);
    } else if (device === "motor_down") {
      indicator.innerHTML =
        "Persiana Bajando <span class='arrow-down'>↓</span>";
      state.innerHTML = "<span class='arrow-down'>↓</span>";
      setTimeout(() => {
        indicator.innerHTML = "Persiana cerrada";
        state.innerHTML = "<span class='window-closed'>🪟</span>";
      }, 1000);
    }
  }
  function controlDevice(device, state) {
    if (device.startsWith("led")) {
      updateLightStatus(device, state);
    } else if (device.startsWith("motor_")) {
      updateServoStatus(device, state);
    }
    fetch("/control", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ [device]: parseInt(state) }),
    });
  }
  function loadLog() {
    const logElement = document.getElementById("log");
    const currentText = logElement.innerText; // Guardar texto actual

    fetch("/log")
      .then((response) => response.text())
      .then((text) => {
        if (text) {
          // Solo actualizar si hay nuevo contenido
          logElement.innerText = text;
        }
      })
      .catch(() => {
        logElement.innerText = currentText; // Restaurar si falla
      });
  }
  window.onload = function () {
    loadLog();
    setInterval(loadLog, 2000);
  };

  function checkTemperature() {
    fetch("/temperature")
      .then((response) => response.text())
      .then((text) => {
        const tempElement = document.getElementById("temperature");
        const tempValue = parseFloat(text);

        // Aplicar estilos base
        tempElement.style.fontSize = "24px";
        tempElement.style.fontWeight = "bold";
        tempElement.style.padding = "15px";
        tempElement.style.borderRadius = "10px";
        tempElement.style.textAlign = "center";
        tempElement.style.margin = "10px 0";
        tempElement.style.transition = "all 0.3s ease";

        if (tempValue > 150) {
          // Estilo para temperatura peligrosa
          tempElement.style.backgroundColor = "#ff4444";
          tempElement.style.color = "white";
          tempElement.style.boxShadow = "0 0 15px #ff0000";
          tempElement.innerHTML = `${text}°C 🔥 <span style="animation: blink 1s infinite;">¡PELIGRO!</span>`;
        } else if (tempValue > 100) {
          // Estilo para temperatura alta
          tempElement.style.backgroundColor = "#ff9966";
          tempElement.style.color = "black";
          tempElement.innerHTML = `${text}°C ⚠️`;
        } else if (tempValue > 50) {
          // Estilo para temperatura moderada
          tempElement.style.backgroundColor = "#ffff99";
          tempElement.style.color = "black";
          tempElement.innerHTML = `${text}°C ☀️`;
        } else {
          // Estilo para temperatura normal
          tempElement.style.backgroundColor = "#99ccff";
          tempElement.style.color = "black";
          tempElement.innerHTML = `${text}°C ❄️`;
        }
      });
  }

  function iniciarShowLuces() {
    fetch("/showluces", { method: "POST" })
      .then(() => alert("¡Show de luces activado! "))
      .catch(() => alert("No se pudo activar el show de luces"));
  }


  function checkForNotification() {
    fetch("/notifications")
      .then((response) => response.text())
      .then((text) => {
        if (text.includes("¡Objeto detectado")) {
          showNotification();
        }
      });
  }

  function showNotification() {
    let notification = document.getElementById("notification");
    notification.style.display = "block";

    setTimeout(() => {
      notification.style.display = "none";
    }, 3000);
  }
  setInterval(checkForNotification, 1000);
  setInterval(checkTemperature, 200000000);

  function actualizarEstadoAlarma() {
    fetch("/alarma")
      .then((response) => response.text())
      .then((estado) => {
        const texto = estado === "on" ? "Activa" : "Desactivada";
        const textoBoton = estado === "on" ? "Desactivar Alarma" : "Activar Alarma";

        document.getElementById("alarma-estado").innerText = texto;
        document.getElementById("alarma-boton").innerText = textoBoton;
      });
  }

  function toggleAlarma() {
    fetch("/alarma")
      .then((response) => response.text())
      .then((estadoActual) => {
        const nuevoEstado = estadoActual === "on" ? "off" : "on";

        fetch("/alarma", {
          method: "POST",
          headers: { "Content-Type": "text/plain" },
          body: nuevoEstado,
        }).then(() => actualizarEstadoAlarma());
      });
  }

  function actualizarLog() {
    fetch("/log")
      .then(response => response.text())
      .then(data => {
        document.getElementById("log").innerText = data;
      });
  }

  setInterval(actualizarLog, 2000);


  function actualizarTemperatura() {
    fetch("/temperature")
      .then(response => response.text())
      .then(data => {
        document.getElementById("temperature").innerText = data + " °C";
      });
  }

  setInterval(actualizarTemperatura, 2000);


  window.onload = function () {
    loadLog();
    actualizarEstadoAlarma();  // Cargar estado de la alarma al inicio
    setInterval(loadLog, 2000);
  };