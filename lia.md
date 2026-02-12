<!--
author:   David Croft
email:    david.croft@warwick.ac.uk
version:  0.1.0
language: en
narrator: US English Male

classroom: false
icon: https://dscroft.github.io/liascript_materials/assets/logo.svg


import: macros_webserial.md

@onload
async function waitForConnection() {
  while (!window.connection) {
    await new Promise(resolve => setTimeout(resolve, 1000)); // wait 100ms
  }

  console.log("Connection available");
}

// Call this function to start the waiting process
waitForConnection();
@end

@webserial_logger
<div id="logger" style="font-size: small"></div>

<script>
window.connection.on('logger', function(msg) 
    {
      console.log("logger: " + msg);
      const logger = document.getElementById("logger");
      const lines = logger.innerHTML.split('<br>');
      const timestamp = new Date().toLocaleTimeString('en-US', { hour12: false, hour: '2-digit', minute: '2-digit', second: '2-digit', fractionalSecondDigits: 3 });
      lines.push(timestamp + ": " + msg);
      if (lines.length > 10) lines.shift();
      logger.innerHTML = lines.join('<br>');
    });
""
</script>
@end

-->


# Arduino setup

This interface requires a specific sketch to be running on the Arduino board.
If you have not already done so, upload it to the board now.

[Go to uploader page](assets/upload.html)

--------------------------

This sketch allows control of 4 servos connected to the following pins.

<!-- data-type="none" -->
| Servo | Pin |
|-|-|
| A | 3 |
| B | 5 | 
| C | 6 |
| D | 9 |


# Servo control

@WebSerial.defaultManager

-------------------------

Use the controls below to test that your servos move as expected.

| Servo | Position | Control |
|-|-|---|
| Servo A | <input id="servoAtext" class="lia-quiz__input" type="number" min="0" max="180" step="1"> | <input type="range" min="0" max="180" value="90" id="servoAslide" /> |
| Servo B | <input id="servoBtext" class="lia-quiz__input" type="number" min="0" max="180" step="1"> | <input type="range" min="0" max="180" value="90" id="servoBslide" /> |
| Servo C | <input id="servoCtext" class="lia-quiz__input" type="number" min="0" max="180" step="1"> | <input type="range" min="0" max="180" value="90" id="servoCslide" /> |
| Servo D | <input id="servoDtext" class="lia-quiz__input" type="number" min="0" max="180" step="1"> | <input type="range" min="0" max="180" value="90" id="servoDslide" /> |

@webserial_logger

<script> 
function send(id,value)
{
  try {
    window.connection.send(id, value);
  } catch (error) {
    console.error("An error occurred:", error); 
  }
}

function slider_update(id)
{
  let value = document.getElementById("servo"+id+"slide").value;
  document.getElementById("servo"+id+"text").valueAsNumber = value;

  send(id, value);
}

function text_update(id)
{
  let value = document.getElementById("servo"+id+"text").value;
  document.getElementById("servo"+id+"slide").valueAsNumber = value;

  send(id, value);
}

for( let i of ['A','B','C','D'] )
{
  document.getElementById("servo"+i+"slide").addEventListener("input", () => slider_update(i));
  document.getElementById("servo"+i+"text").addEventListener("input", () => text_update(i));
  slider_update(i);
}
</script>


# Control Sequences

The following page will allow you to enter a sequence of servo positions to iterate through in sequence.

- Servos positions are specified with a single character followed by angle.

  - A100 for example sets servo A to 100°.
  - B90 sets servo B to 90° etc.

- Pauses can be specified using P followed by the number of milliseconds.

  - P1000 for example will wait 1 second before progressing.

- Comments can be included if prefixed with the % character.

  - % comment goes here. 

- Commands can be placed on a single or multiple lines, with or without spaces. E.g:

  - A100 B120 C130 P1000
  - A100B120C130P1000
  - A100, B120, C130, P1000


## Sequence Editor

@WebSerial.defaultManager

-------------------------

Example sequence:

- A0 P2000 A180 P2000 A0 P2000


<script id="run" input="submit" default="Start/Stop sequence">
  function process_sequence()
  {
    console.log("process_sequence " + window.sequence );

    if( !window.sequence_running ) 
    {
      return;
    }

    if( window.sequence.length > 0 )
    {
      let next = window.sequence[0];
      window.sequence.shift(); // pop

      if( next[0] == "P")
      {
        setTimeout(process_sequence, parseInt(next.substring(1)));
      }
      else
      {
        try {
          window.connection.send(next[0], next.substring(1));
        } catch (error) {
          console.error("An error occurred:", error); 
        }

        setTimeout(process_sequence, 5);
      }
    }
    else
    {
      window.sequence_running = false;
    }
  }

  let text = document.getElementById("sequence").value;

  // Remove comments (lines or parts starting with %)
  let lines = text.toUpperCase().split('\n').map(line => {
    let index = line.indexOf('%');
    return index !== -1 ? line.substring(0, index) : line;
  }).join('\n');


  let regex = /([A-DP])(\d+)/g;
  let matches = lines.matchAll(regex);
  let commands = Array.from(matches).map(match => match[0]);

  window.sequence = commands;
  window.sequence_running = true;

  process_sequence();
</script> 

<textarea id="sequence" class="lia-input lia-quiz__input" rows="10" placeholder="Enter some text..."></textarea>

<script>

  document.getElementById("sequence").addEventListener("input", function() {
    document.cookie = "sequence=" + encodeURIComponent(this.value) + "; path=/";
  });

  // Load sequence from cookie on page load

  const cookies = document.cookie.split(';').map(c => c.trim());
  const sequenceCookie = cookies.find(c => c.startsWith('sequence='));
  if (sequenceCookie) {
    document.getElementById("sequence").value = decodeURIComponent(sequenceCookie.split('=')[1]);
  }
  ""
</script>

@webserial_logger
