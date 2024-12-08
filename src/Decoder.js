import React, { useState } from 'react';
import './encoder.css';
import './App.css';

function Decoder() {
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
        
       var temp=text.split('\n');
       if(temp[temp.length-1]==="")
       {temp.pop();}
       for(var i=0;i<temp.length;i++)
       {
        if (temp[i].length!==32 && temp[i].length>0)
        {
            alert("all ins must be 32 bits and after 32 bits there must be enter");
            return;
        }
        for(var j=0;j<32;j++)
        {
            if((temp[i][j]!=='0' &&  temp[i][j]!=='1'))
            {  console.log(i,j);
                alert('only 0/1 are valid');
                return;
            }
        }
       }
        const result = Module.ccall('ins_decoder', 'string', ['string'], [text]);
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
        <a  href="/">Home</a>
        <a href="/encoder">Encoder</a>
        <a href="/pipeline">Pipeline</a>
      </div>
      <div className='encoder-text'>
        primary task of Decoder is to convert 32-bit binary instructions to assembly code
        <div style={{ alignSelf: 'center' }}>
         
        </div>
      </div>
      <div className='encoder-text' style={{ fontSize: '22px' }}>
        Convertable Instructions<br />
        Basic-set: R, I, Load, Store, Branch<br />
        u-type: lui<br />
        extended-set: mul, div, rem<br/>
      </div>
      <div className='encoder-main'>
        <div className='encoder-input'>
          <pre>{getLineNumbers()}</pre>
          
          <textarea
            value={text}
            onChange={Handler}
            placeholder='Your machine code instructions...'
          />
        </div>
        <div id="between">
        <button onClick={Submit}>Generate</button>
        </div>
        <div className='encoder-output'>
          <pre>{getLineNumbers2()}</pre>
          <div>
            {output.split('\n').map((line, index) => {

              return <span key={index}>{line}<br/></span>;
            })}
          </div>
        </div>
      </div>
    </div>
  );
}

export default Decoder;
