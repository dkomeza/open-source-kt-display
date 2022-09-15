import React, { useRef, useState } from 'react';
import './scss/style.scss'

function App() {
  const [code, setCode] = useState([98, 123, 213])
  const b12Ref = useRef<HTMLInputElement>(null);
  
  function handleInput() {
    setCode(code => [...code, Number(b12Ref.current?.value)])
  }

  return (
    <div className="App">
      <header>
        <h1>Open Source KT Display</h1>
      </header>
      <main>
        <section>
          <form onInput={handleInput}>
            <input type={"number"} ref={b12Ref}></input>
          </form>
          <div>{ code.join(" ") }</div>
        </section>
      </main>
    </div>
  );
}

export default App;
