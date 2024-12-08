import React, { useState } from 'react';
import './encoder.css';
import './App.css';

function Encoder() {
  const [text, setText] = useState("");
  const [output, setOutput] = useState("");


 


  const Handler = (e) => {
    setText(e.target.value);
  };

  const getLineNumbers = () => {
    const lines = text.split("\n").length;
    return Array.from({ length: lines }, (_, i) => i + 1).join("\n");
  };

  const getLineNumbers2 = () => {
    const lines = output.split("\n").length;
    return Array.from({ length: lines }, (_, i) => i + 1).join("\n");
  };

  const Submit = async (e) => {
    e.preventDefault();
    const script = document.createElement('script');
    script.src = `${process.env.PUBLIC_URL}/d1.js`;
    script.async = true;

    script.onload = async () => {
      if (window.createModule) {
        const Module = await window.createModule();
        if(text==="" || text==="\n")
        {
          alert("write something");
          return;
        }
        const result = Module.ccall('decoder', 'string', ['string'], [text]);
        setOutput(result);
        console.log(output);
      } else {
        console.error('createModule is not defined. Check the Emscripten output.');
      }
    };

    script.onerror = () => {
      console.error('Failed to load the WebAssembly module.');
    };

    document.body.appendChild(script);

   
  };

  return (
    <div>
      <div className='encoder-menu'>
        <a href="/">Home</a>
        <a href="/decoder">Decoder</a>
        <a  href="/pipeline">Pipeline</a>
      </div>
      <div className='encoder-text'>
        primary task of Encoder is to convert assembly code to machine/binary code
        <div style={{ alignSelf: 'center' }}>
         
        </div>
      </div>
      <div className='encoder-text' style={{ fontSize: '22px' }}>
        Convertable Instructions<br />
        Basic-set: R, I, Load, Store, Branch<br />
        u-type: lui<br />
        extended-set: mul, div, rem<br />
      </div>
      <div className='encoder-main'>
        <div className='encoder-input'>
          <pre>{getLineNumbers()}</pre>
          
          <textarea
            value={text}
            onChange={Handler}
            placeholder='Your assembly code can go here'
          />
        </div>
        <div id="between">
        <button onClick={Submit} type="button">Generate</button>
        <span className='c-red'>function 7</span>
                    <span className='c-blue'>Rs2</span>
                    <span className='c-green'>Rs1</span>
                    <span className='c-pink'>function 3</span>
                    <span className='c-orange'>Rd</span>
                    <span className='c-brown'>opcode</span>
                    <span className='c-gray'>Immediate</span>
        </div>
        <div className='encoder-output'>
          <pre>{getLineNumbers2()}</pre>
          <div>
            {output.split('\n').map((line, index) => {
              const opcode = line.slice(25, 32);
              if (opcode === "0110011") {
                return (
                  <span key={index}>
                    <span className='c-red'>{line.slice(0, 7)}</span>
                    <span className='c-blue'>{line.slice(7, 12)}</span>
                    <span className='c-green'>{line.slice(12, 17)}</span>
                    <span className='c-pink'>{line.slice(17, 20)}</span>
                    <span className='c-orange'>{line.slice(20, 25)}</span>
                    <span className='c-brown'>{line.slice(25, 32)}</span>
                    <br />
                  </span>
                );
              }
              if (opcode === "0010011" || opcode === "0000011") {
                return (
                  <span key={index}>
                    <span className='c-gray'>{line.slice(0, 12)}</span>
                    <span className='c-green'>{line.slice(12, 17)}</span>
                    <span className='c-pink'>{line.slice(17, 20)}</span>
                    <span className='c-orange'>{line.slice(20, 25)}</span>
                    <span className='c-brown'>{line.slice(25, 32)}</span>
                    <br />
                  </span>
                );
              }
              if (opcode === "0100011" || opcode === "1100011") {
                return (
                  <span key={index}>
                    <span className='c-gray'>{line.slice(0, 7)}</span>
                    <span className='c-blue'>{line.slice(7, 12)}</span>
                    <span className='c-green'>{line.slice(12, 17)}</span>
                    <span className='c-pink'>{line.slice(17, 20)}</span>
                    <span className='c-gray'>{line.slice(20, 25)}</span>
                    <span className='c-brown'>{line.slice(25, 32)}</span>
                    <br />
                  </span>
                );
              }
              if (opcode === "0110111") {
                return (
                  <span key={index}>
                    <span className='c-gray'>{line.slice(0, 20)}</span>
                    <span className='c-orange'>{line.slice(20, 25)}</span>
                    <span className='c-brown'>{line.slice(25, 32)}</span>
                    <br />
                  </span>
                );
              }

              return <span key={index}>{line}</span>;
            })}
          </div>
        </div>
      </div>
    </div>
  );
}

export default Encoder;
