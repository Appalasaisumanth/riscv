/* eslint-disable no-unused-vars */
import React, { useEffect, useState } from 'react';
import './encoder.css';
import './App.css';

function Pipeline() {
  const [text, setText] = useState("");
  const [logs, setLogs] = useState([]);
  const [data, SetData] = useState([]);

  const Handler = (e) => {
    setText(e.target.value);
  };

  const getLineNumbers = () => {
    const lines = text.split("\n").length;
    return Array.from({ length: lines }, (_, i) => i + 1).join("\n");
  };


  const Submit = async () => {
  
    const script = document.createElement('script');
    script.src = `${process.env.PUBLIC_URL}/d1.js`;
    script.async = true;

    script.onload = async () => {
      if (window.createModule) {
        const Module = await window.createModule();
        if (text === "" || text === "\n") {
          alert("write something");
          return;
        }

        if (text === "" || text === "\n") {
          alert("write something");
          return;
        }
        setLogs([]);
        const result = Module.ccall('pipeline', 'string', ['string'], [text]);

        const originalConsoleLog = console.log;

        // Override console.log
        console.log = (...args) => {
          // Call the original console.log
          originalConsoleLog.apply(console, args);
          const message = args.map(arg => (typeof arg === 'object' ? JSON.stringify(arg) : arg)).join(' ');
          setLogs(prevLogs => [...prevLogs, message]);
          console.log = originalConsoleLog;
        };

        if (logs.length > 0) {
          let dummydata = [[" ", " ", " ", " ", " "]];
          let map={
            "FETCH":0,
            "DECODE":1,
            "Execute":2,
            "Memory":3,
            "WRITE-BACK":4
          }
        
          for (var i = 0; i < logs.length; i++) {
            let tem = logs[i].split(" ");
            tem=tem.filter(item=>item!=='');
            if (tem[0][0]==='c')
            {
              var cycle=Number(tem[0].split('cycle')[1]);
              if (dummydata.length===cycle)
              {
                dummydata[cycle-1][map[tem[1]]]=tem[2]+" "+tem[3];
                
              }
              else
              {
                dummydata.push(["","","","",""]);
                dummydata[cycle-1][map[tem[1]]]=tem[2]+" "+tem[3];
               
              }
            }
            else if(tem[0][0]==="E")
            {
              dummydata[dummydata.length-1][3]+="\n"+tem.join(" ");
            }
      

          }
          SetData(dummydata);
        }

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
        <a href="/encoder">Encoder</a>
        <a href="/decoder">Decoder</a>
      </div>
      <div className='encoder-text'>
        primary task of This pipeline is to reduce time for total instructions without distrubing ipc
        <div style={{ alignSelf: 'center' }}>

        </div>
      </div>
      <div className='encoder-text' style={{ fontSize: '22px' }}>
        Executable Instructions<br />
        Basic-set: R, I, Load, Store, Branch<br />
        u-type: lui<br />
        extended-set: mul, div, rem<br />
        Memory limit is 4096B with 1024 words starts from 0<br/>
        please triple click Generate button to see result
        <div style={{ marginRight: 'auto', marginLeft: 'auto' }}>
          <button type='button' onClick={(e)=>{Submit();}}>Generate</button>
        </div>
      </div>


      <div className='encoder-main'>

        <div className='encoder-input'>
          <pre>{getLineNumbers()}</pre>

          <textarea
            value={text}
            onChange={Handler}
            placeholder='Your assembly code ...'
          />
        </div>

        <div className='encoder-output' style={{ border: '1px solid #ccc', overflowY: 'auto' }}>
          <pre>Log Output:</pre>
          <div>
            {logs.map((log, index) => (
              <div key={index}>{log}</div>
            ))}
          </div>
        </div>

      </div>
      {data.length>0 && (<div className='encoder-table'>
      <h1>Timing Diagram</h1>
      <table border="1">
        <thead>
          <tr>
            <th>Cycle</th>
            <th>FETCH</th>
            <th>DECODE</th>
            <th>EXECUTE</th>
            <th>MEMORY</th>
            <th>WRITE-BACK</th>
          </tr>
        </thead>
        <tbody>
          {data.map((log, index) => (
            <tr key={index}>
              <td>{index+1}</td>
              <td>{log[0]}</td>
              <td>{log[1]}</td>
              <td>{log[2]}</td>
              <td>{log[3]}</td>
              <td>{log[4]}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>)}
      

    </div>
  );
}

export default Pipeline;
