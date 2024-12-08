
import './App.css';
import { React } from 'react';
function App() {
 
 
  return (
    <div className="App">
      <div className='App-header'>
        <div className='App-logo'>
       
         <img src="https://img.electronicdesign.com/files/base/ebm/electronicdesign/image/2020/11/RISC_V_logo.5fa1b8aab3304.png?auto=format%2Ccompress&w=640&width=640"
        alt="riscv-logo"/>
        RISCV-WEB-SIMULATOR 
        
        </div>
        <div className='App-menu'>
          <a href="#about" >About</a>
          <a href="/encoder" >Encoder</a>
          <a href="/decoder" >Decoder</a>
          <a href="/pipeline" >Pipeline</a>
        </div>
        </div>
        <div className='app-main'>
          <div className='pipeline-img'>
            The outline of a 5 stage pipeline Design
            <img src="https://www.researchgate.net/profile/Sajjad-Ahmed-23/publication/355051535/figure/fig3/AS:1076240400289792@1633607115859/The-RISC-V-ISA-compliant-RV32IM-5-Stage-fully-pipelined-datapath-designed-from-scratch.ppm" alt="5-stage-pipeline-design"/>
          </div>
          <div className='pipeline-iframe'>
            All R-type,I-type,Branch,Load,store,lui Instructions are from  basic riscv RV32I  <br/>
            mul,rem,div from RV32M(multiplication extension set)
            <iframe title="riscv-green-card" src="/RISCV_CARD_merged.pdf">

            </iframe>
          </div>
        <div id="about">
          Course work of Computer-organisation and architechture(COA) course of IIT-Bhubhaneshwar.<br/>
          This all about for running riscv code(set of instructions) in a 5 staged pipeline design<br/>
          There are various parts encoding the instruction with encoder by following the Manual
          executing the instruction in 5 staged pipeline <br/>
          This frontend is  connected to c++ backend with web-assembly <br/>
          <div id="src">
            <div>
          source code: <a href="https://github.com/Appalasaisumanth/riscv">GITHUB</a>
          </div>
          <div>
          contact me : <a href='mailto:appalasaisumanth@gmail.com'> E-mail</a>
          </div>
          </div>
        </div>
      </div>
      
      
    </div>
  );
}

export default App;
