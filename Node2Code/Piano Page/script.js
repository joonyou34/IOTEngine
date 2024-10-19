// Array of white keys (C to B) and their indices
const whiteKeys = [
    'C', 'D', 'E', 'F', 'G', 'A', 'B', 
    'C', 'D', 'E', 'F', 'G', 'A', 'B'
];
const blackKeys = [
    'C#', 'D#', 'F#', 'G#', 'A#', 
    'C#', 'D#', 'F#', 'G#', 'A#'
];

const ipaddress = "192.168.1.69";

// Array of black key positions relative to the white keys
const blackKeyPositions = [1, 2, 4, 5, 6, 8, 9, 11, 12, 13]; // Corresponds to the white key positions
const whiteidxes = [0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23];
const blackidxes = [1, 3, 6, 8, 10, 13, 15, 18, 20, 22];

const keyMapping = {
    'q': 0, 'Q': 1, 'w': 2, 'W': 3, 'e': 4, 'r': 5, 'R': 6,
    't': 7, 'T': 8, 'y': 9, 'Y': 10, 'u': 11, 'i': 12, 'I': 13,
    'o': 14, 'O': 15, 'p': 16, 'a': 17, 'A': 18, 's': 19,
    'S': 20, 'd': 21, 'D': 22, 'f': 23
};

const piano = document.getElementById('piano');
const activeKeys = {};
const reconnectInterval = 5000;
let shiftActive = false;
let lastNote = -1;
let isConnected = false;
let socket = null;
let initialized = false;

let mouseDown = false;

// Function to create key event listeners
function createKeyListeners(keyElement, index, type) {
    let keyActivated = false;
    let keyPressed = false;

    const handleMouseDown = (event) => {
        event.preventDefault();
        mouseDown = true;
        if (!keyActivated) {
            handleKeyPress(type === 'white' ? whiteidxes[index] : blackidxes[index], 1);
            keyActivated = true;
            keyPressed = true;
        }
    };

    const handleMouseUp = () => {
        mouseDown = false;
        handleKeyPress(type === 'white' ? whiteidxes[index] : blackidxes[index], 0);
        keyActivated = false;
        keyPressed = false;
    };

    const handleMouseLeave = () => {
        if (mouseDown && keyPressed) {
            handleKeyPress(type === 'white' ? whiteidxes[index] : blackidxes[index], 0);
            keyPressed = false;
        }
        keyActivated = false;
    };

    const handleMouseMove = () => {
        if (mouseDown && !keyActivated) {
            handleKeyPress(type === 'white' ? whiteidxes[index] : blackidxes[index], 1);
            keyActivated = true;
            keyPressed = true;
        }
    };

    keyElement.addEventListener('mousedown', handleMouseDown);
    keyElement.addEventListener('mouseup', handleMouseUp);
    keyElement.addEventListener('mouseleave', handleMouseLeave);
    keyElement.addEventListener('mousemove', handleMouseMove);

    // Touch event listeners
    keyElement.addEventListener('touchstart', (event) => {
        event.preventDefault();
        handleMouseDown(event);
    });

    keyElement.addEventListener('touchend', handleMouseUp);
    keyElement.addEventListener('touchcancel', handleMouseUp);
    keyElement.addEventListener('touchmove', (event) => {
        const touch = event.touches[0];
        const rect = keyElement.getBoundingClientRect();

        // Check if the touch point is within the key's bounding box
        if (touch.clientX >= rect.left && touch.clientX <= rect.right && 
            touch.clientY >= rect.top && touch.clientY <= rect.bottom) {
            if (!keyActivated) {
                handleKeyPressLogic(true); // Press the key
            }
        } else {
            if (keyPressed) {
                handleKeyPressLogic(false); // Release the key if leaving
            }
        }
    });
}

// Create white keys
whiteKeys.forEach((note, index) => {
    const whiteKey = document.createElement('div');
    whiteKey.classList.add('key', 'white');
    whiteKey.setAttribute('data-note', note);
    whiteKey.setAttribute('data-index', whiteidxes[index]);
    createKeyListeners(whiteKey, index, 'white');
    piano.appendChild(whiteKey);
});

// Create black keys with spacing and positioning
blackKeyPositions.forEach((position, index) => {
    const blackKey = document.createElement('div');
    blackKey.classList.add('key', 'black');
    blackKey.setAttribute('data-index', blackidxes[index]);
    blackKey.setAttribute('data-note', blackKeys[index]);
    blackKey.style.left = `${position * 43.5}px`;
    createKeyListeners(blackKey, index, 'black');
    piano.appendChild(blackKey);
});

// Global mouseup listener to reset mouseDown state
document.addEventListener('mouseup', () => {
    mouseDown = false;
});

// Function to send the HTTP request
function sendRequest(index, type) {
    const url = `http://${ipaddress}/piano?index=${index}&type=${type}`;
    fetch(url);
}

// Function to handle key press or release
function handleKeyPress(index, type) {
    const keyElement = document.querySelector(`.key[data-index='${index}']`);
    if (type === 1) {
        keyElement.classList.add('active');
    } else {
        keyElement.classList.remove('active');
    }
    sendRequest(index, type);
}

// Add keyboard event listeners for keydown and keyup
document.addEventListener('keydown', (event) => {
    const key = event.key;
    if (keyMapping.hasOwnProperty(key) && !activeKeys[key]) {
        const index = keyMapping[key];
        activeKeys[key] = true;
        handleKeyPress(index, 1);
    }

    if (event.key === 'Shift') {
        shiftActive = true;
    }
});

document.addEventListener('keyup', (event) => {
    const key = event.key;
    if (keyMapping.hasOwnProperty(key) && activeKeys[key]) {
        const index = keyMapping[key];
        handleKeyPress(index, 0);
        activeKeys[key] = false;
    }

    if (event.key === 'Shift') {
        shiftActive = false;
    }
});

// Add a global keyup event listener to handle situations where the Shift key is released
document.addEventListener('keyup', (event) => {
    if (event.key === 'Shift') {
        Object.keys(activeKeys).forEach((key) => {
            if (activeKeys[key]) {
                const index = keyMapping[key];
                handleKeyPress(index, 0);
                activeKeys[key] = false;
            }
        });
    }
});

// Functions for recording and playback
function adjustRecordButton(text) {
    const button = document.getElementById("record-button");
    button.textContent = text == "1" ? "Stop Recording" : "Start Recording";
    button.classList.toggle("pressed", text == "1");
}

function adjustPlaybackButton(text) {
    const button = document.getElementById("playback-button");
    button.textContent = text == "1" ? "Stop Playback" : "Start Playback";
    button.classList.toggle("pressed", text == "1");
}

function record() {
    const url = `http://${ipaddress}/record`;
    fetch(url)
        .then(response => response.ok ? response.text() : null)
        .then(text => adjustRecordButton(text));
}

function playback() {
    const url = `http://${ipaddress}/playback`;
    fetch(url)
        .then(response => response.ok ? response.text() : null)
        .then(text => adjustPlaybackButton(text));
}

// WebSocket connection
function attemptReconnect() {
    if (!isConnected) {
        setTimeout(socketConnect, reconnectInterval);
    }
}

function socketConnect() {
    socket = new WebSocket(`ws://${ipaddress}:81`);
    socket.onopen = () => {
        isConnected = true;
    };
    
    socket.onclose = () => {
        isConnected = false;
        attemptReconnect();
    };
    
    socket.addEventListener('message', (event) => {
        const nextNote = parseInt(event.data);
        if (nextNote != -1) {
            const keyElement = document.querySelector(`.key[data-index='${nextNote}']`);
            keyElement.classList.add('active');
        }
        if (lastNote != -1) {
            const keyElement = document.querySelector(`.key[data-index='${lastNote}']`);
            keyElement.classList.remove('active');
        }
        lastNote = nextNote;
    });
}

// Initial fetch and socket connection
fetch(`http://${ipaddress}/`).then(response => {
    if (response.ok) return response.text();
})
.then(text => {
    adjustRecordButton(text[0]);
    adjustPlaybackButton(text[1]);
});

socketConnect();
