import React, { useEffect, useRef, useState } from 'react';
import './scss/style.scss'

function App() {
  const [code, setCode] = useState([12,0,149,160,41,102,18,74,4,20,0,50,14])
  const [binaryCode, setBinaryCode] = useState<Array<String> | null>(null)
  const refList: any = {0: useRef<HTMLInputElement | null>(null), 1: useRef<HTMLInputElement | null>(null), 2: useRef<HTMLInputElement | null>(null), 3: useRef<HTMLInputElement | null>(null), 4: useRef<HTMLInputElement | null>(null), 5: useRef<HTMLInputElement | null>(null), 6: useRef<HTMLInputElement | null>(null), 7: useRef<HTMLInputElement | null>(null), 8: useRef<HTMLInputElement | null>(null), 9: useRef<HTMLInputElement | null>(null), 10: useRef<HTMLInputElement | null>(null), 11: useRef<HTMLInputElement | null>(null), 12: useRef<HTMLInputElement | null>(null)}

  useEffect(() => {
    let codeTemp = []
    for (let i = 0; i < code.length; i++) {
      codeTemp[i] = code[i].toString(2)
    }
    setBinaryCode(codeTemp)
  }, [code])

  useEffect(() => {
    handleInput()
  }, [])
  
  function handleInput() {
    let tempCode = []
    for (let i:number = 0; i < code.length ;i++) {
      tempCode[i] = Number(refList[i].current?.value) || 0
    }
    let temp = []
    for (let i = 0; i < code.length; i++) {
      temp.push(`<span><label htmlFor="b${i}Ref"></label><input id="b${i}Ref" type={"number"} ref={refList[${i}]} defaultValue={${code[i]}}></input></span>`)
    }
    console.log(temp.join("\n"))
    setCode(tempCode)
    console.log(refList[0].current?.value)
    // setCode(code => [...code, Number(b12Ref.current?.value)])
  }

  return (
    <div className="App">
      <header>
        <h1>Open Source KT Display</h1>
      </header>
      <main>
        <section>
          <form onInput={handleInput}>
            <span><label htmlFor='speed'>Speed Limit</label><input id='speed' type={"number"} /></span>
            <span><label htmlFor='wheel-size'>Wheel Size</label><select id='wheel-size'>
              <option value={"10"}>10"</option>
              <option value={"12"}>12"</option> 
              <option value={"14"}>14"</option> 
              <option value={"16"}>16"</option> 
            </select></span>
            <span><label htmlFor='P1'>P1 Parameter</label><input id='P1' type={"number"} /></span>
            {/* <span><label htmlFor="b0Ref"></label><input id="b0Ref" type={"number"} ref={refList[0]} defaultValue={12}></input></span>
            <span><label htmlFor="b1Ref"></label><input id="b1Ref" type={"number"} ref={refList[1]} defaultValue={0}></input></span>
            <span><label htmlFor="b2Ref"></label><input id="b2Ref" type={"number"} ref={refList[2]} defaultValue={149}></input></span>
            <span><label htmlFor="b3Ref"></label><input id="b3Ref" type={"number"} ref={refList[3]} defaultValue={160}></input></span>
            <span><label htmlFor="b4Ref"></label><input id="b4Ref" type={"number"} ref={refList[4]} defaultValue={41}></input></span>
            <span><label htmlFor="b5Ref"></label><input id="b5Ref" type={"number"} ref={refList[5]} defaultValue={102}></input></span>
            <span><label htmlFor="b6Ref"></label><input id="b6Ref" type={"number"} ref={refList[6]} defaultValue={18}></input></span>
            <span><label htmlFor="b7Ref"></label><input id="b7Ref" type={"number"} ref={refList[7]} defaultValue={74}></input></span>
            <span><label htmlFor="b8Ref"></label><input id="b8Ref" type={"number"} ref={refList[8]} defaultValue={4}></input></span>
            <span><label htmlFor="b9Ref"></label><input id="b9Ref" type={"number"} ref={refList[9]} defaultValue={20}></input></span>
            <span><label htmlFor="b10Ref"></label><input id="b10Ref" type={"number"} ref={refList[10]} defaultValue={0}></input></span>
            <span><label htmlFor="b11Ref"></label><input id="b11Ref" type={"number"} ref={refList[11]} defaultValue={50}></input></span>
            <span><label htmlFor="b12Ref"></label><input id="b12Ref" type={"number"} ref={refList[12]} defaultValue={14}></input></span> */}
          </form>
          <div>{ code.join(" ") }</div>
          <div>{ binaryCode?.join(" ") }</div>
        </section>
      </main>
    </div>
  );
}

export default App;
