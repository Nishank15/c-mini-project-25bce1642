const outputElement = document.getElementById('output');
const terminalWindow = document.getElementById('terminal-window');
const cmdInput = document.getElementById('cmd-input');
const enterBtn = document.getElementById('enter-btn');

let inputResolve = null; // This will hold our "pause" state

// This function is called directly by our C code via EM_ASYNC_JS
window.waitForUserInput = function() {
    return new Promise((resolve) => {
        inputResolve = resolve; // Save the resolve function
        cmdInput.disabled = false;
        cmdInput.focus();
    });
};

var Module = {
    print: function(text) {
        if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
        outputElement.innerHTML += text + '\n';
        terminalWindow.scrollTop = terminalWindow.scrollHeight;
    },
    printErr: function(text) {
        if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
        outputElement.innerHTML += '<span style="color: #ef4444">' + text + '</span>\n';
        terminalWindow.scrollTop = terminalWindow.scrollHeight;
    }
};

function submitCommand() {
    // If C isn't currently asking for input, do nothing
    if (!inputResolve) return;

    const cmd = cmdInput.value.trim();

    if (cmd === '0') {
        cmdInput.disabled = true;
        cmdInput.placeholder = "Session ended. Refresh page to restart.";
    }
    
    // Visually style the user's input in white/bold in the terminal
    Module.print('<span style="color: #f8fafc; font-weight: bold;">' + cmd + '</span>');

    const resolveFunc = inputResolve;
    inputResolve = null; // Reset state
    cmdInput.value = '';
    
    // Resume the C program and hand it the text!
    resolveFunc(cmd);
}

enterBtn.addEventListener('click', submitCommand);
cmdInput.addEventListener('keypress', function(e) {
    if (e.key === 'Enter') submitCommand();
});

// --- RUN DEMO LOGIC ---
const demoBtn = document.getElementById('demo-btn');

// The sequence of inputs to simulate
const demoCommands = [
    "1", "101", "Arun Kumar", "45", "9876543210", // Register Arun
    "1", "102", "Priya Nair", "32", "8765432109", // Register Priya
    "3", "101",                                   // Search Arun
    "2", "101", "10-03-2026", "Hypertension", "Amlodipine 5mg", // Add visit
    "4", "101"                                    // View history
];

async function runDemo() {
    // Disable inputs while demo is running
    demoBtn.disabled = true;
    cmdInput.disabled = true;
    enterBtn.disabled = true;

    for (let i = 0; i < demoCommands.length; i++) {
        // 1. Wait until C code is ready for input (inputResolve exists)
        while (!inputResolve) {
            await new Promise(resolve => setTimeout(resolve, 100));
        }

        // 2. Pause slightly for human-like reading pace
        await new Promise(resolve => setTimeout(resolve, 800));

        // 3. Type the command into the input box
        cmdInput.value = demoCommands[i];

        // 4. Pause slightly before hitting enter
        await new Promise(resolve => setTimeout(resolve, 300));

        // 5. Submit the command
        submitCommand();
    }

    // Re-enable inputs when finished
    demoBtn.disabled = false;
    cmdInput.disabled = false;
    enterBtn.disabled = false;
    cmdInput.focus();
}

demoBtn.addEventListener('click', runDemo);