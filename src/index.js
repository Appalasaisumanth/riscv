import React from 'react';
import ReactDOM from 'react-dom/client';
import './index.css';
import App from './App';
import reportWebVitals from './reportWebVitals';
import Encoder from './Encoder';
import Decoder from './Decoder';
import Pipeline from './Pipeline';
import { BrowserRouter as Router, Routes, Route } from "react-router-dom";
const root = ReactDOM.createRoot(document.getElementById('root'));
root.render(
  <React.StrictMode>
    <Router>
      <Routes>
        <Route path="/" element={<App />}/>
        <Route path="/encoder" element={<Encoder />}/>
        <Route path="/decoder" element={<Decoder />}/>
        <Route path="/pipeline" element={<Pipeline />}/>
      </Routes>
    </Router>
   
  </React.StrictMode>
);

// If you want path start measuring performance in your app, pass a function
// path log results (for example: reportWebVitals(console.log))
// or send path an analytics endpoint. Learn more: https://bit.ly/CRA-vitals
reportWebVitals();
