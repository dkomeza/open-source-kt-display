import React, { useEffect, useRef, useState } from 'react';
import './scss/style.scss'

function App() {
  const [code, setCode] = useState<number[] | null>([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
  const [c4check, setC4check] = useState<number>(0)
  const refList: any = {"speed": useRef<HTMLInputElement | null>(null), "wheel-size": useRef<HTMLInputElement | null>(null), "p1": useRef<HTMLInputElement | null>(null), "p2": useRef<HTMLInputElement | null>(null), "p3": useRef<HTMLInputElement | null>(null), "p4": useRef<HTMLInputElement | null>(null), "p5": useRef<HTMLInputElement | null>(null), "c1": useRef<HTMLInputElement | null>(null), "c2": useRef<HTMLInputElement | null>(null), "c4": useRef<HTMLInputElement | null>(null), "c4-secondary": useRef<HTMLInputElement | null>(null), "c5": useRef<HTMLInputElement | null>(null), "c11": useRef<HTMLInputElement | null>(null), "c12": useRef<HTMLInputElement | null>(null), "c13": useRef<HTMLInputElement | null>(null), "c14": useRef<HTMLInputElement | null>(null)}

  // useEffect(() => {
  //   let codeTemp = []
  //   for (let i = 0; i < code.length; i++) {
  //     codeTemp[i] = code[i].toString(2)
  //   }
  // }, [code])

  useEffect(() => {
    handleInput()
  }, [])
  
  function handleInput() {
    setC4check(Number(refList["c4"].current?.value))
    let b2b4Temp = []
    let tempCode = []

    console.log(Number(refList["speed"].current?.value).toString(2))
    let speedTemp = Number(refList["speed"].current?.value).toString(2)
    console.log(speedTemp)
    let wheelSizeTemp = Number(refList["wheel-size"].current?.value).toString(2)
    speedTemp = fillZeroes(speedTemp, 8)
    console.log(speedTemp)
  
    for (let i = 0; i < 4; i++) {
      b2b4Temp[i] = Number(refList["speed"].current?.value).toString(2)[i + 1]
    }
    for (let i = 0; i < 5; i++) {
      b2b4Temp[i] = Number(refList["wheel-size"].current?.value).toString(2)[i]
    }
    b2b4Temp[10] = Number(refList["p1"].current?.value).toString(2)[0]
    console.log(b2b4Temp)

    tempCode[0] = parseInt(refList["p5"].current?.value || "0")
    tempCode[1] = 0 // Handled by the display
    tempCode[2] = 
    tempCode[3] = Number(refList["p1"].current?.value)
    tempCode[4] = 0
    tempCode[5] = 0
    tempCode[6] = ((Number(refList["c1"].current?.value)) << 3) | Number(refList["c2"].current?.value)
    tempCode[7] = ((Number(refList["c14"].current?.value)) << 5) | Number(refList["c5"].current?.value) | 128
    tempCode[8] = ((Number(refList["c4"].current?.value)) << 5) | Number(refList["c12"].current?.value)
    tempCode[9] = Number(refList["c4-secondary"].current?.value) ? Number(refList["c4-secondary"].current?.value) : 20  
    tempCode[10] = ((Number(refList["c13"].current?.value)) << 2) | 1
    tempCode[11] = 50
    tempCode[12] = 14

    
    // console.log(Number(refList["c2"].current?.value << 3).toString(2))
    // for (let i:number = 0; i < code.length ;i++) {
    //   tempCode[i] = Number(refList[i].current?.value) || 0
    // }
    setCode(tempCode)
    // console.log(refList["speed"].current?.value)
    // setCode(code => [...code, Number(b12Ref.current?.value)])
  }

  function fillZeroes(binaryStr: string, length: number) {
    while(binaryStr.length < length) {
      binaryStr = "0" + binaryStr;
    }
    return binaryStr
  }

  return (
    <div className="App">
      <header>
        <h1>Open Source KT Display</h1>
      </header>
      <main>
        <section>
          <form onInput={handleInput}>
            <span><label htmlFor='speed'>Speed Limit</label><input id='speed' type={"number"} defaultValue={72} min={10} max={72} ref={refList["speed"]} /></span>
            <span><label htmlFor='wheel-size'>Wheel Size</label><select id='wheel-size' ref={refList["wheel-size"]} >
              <option value={10}>10"</option>
              <option value={12}>12"</option> 
              <option value={14}>14"</option> 
              <option value={16}>16"</option> 
            </select></span>
            <span><label htmlFor='P1'>P1 Parameter</label><input id='P1' type={"number"} defaultValue={86} min={0} max={255} ref={refList["p1"]} /></span>
            <span><label htmlFor='P2'>P2 Parameter</label><input id='P2' type={"number"} defaultValue={1} min={0} max={6} ref={refList["p2"]} /></span>
            <span><label htmlFor='P3'>P3 Parameter</label><input id='P3' type={"number"} defaultValue={1} min={0} max={1} ref={refList["p3"]} /></span>
            <span><label htmlFor='P4'>P4 Parameter</label><input id='P4' type={"number"} defaultValue={0} min={0} max={1} ref={refList["p4"]} /></span>
            <span><label htmlFor='P5'>P5 Parameter</label><input id='P5' type={"number"} defaultValue={13} min={0} max={64} ref={refList["p5"]} /></span>
            <span><label htmlFor='C1'>C1 Parameter</label><input id='C1' type={"number"} defaultValue={5} min={0} max={7} ref={refList["c1"]} /></span>
            <span><label htmlFor='C2'>C2 Parameter</label><input id='C2' type={"number"} defaultValue={0} min={0} max={1} ref={refList["c2"]} /></span>
            <span><label htmlFor='C4'>C4 Parameter</label><span className='c4-wrapper'><input id='C4' type={"number"} defaultValue={0} min={0} max={4} ref={refList["c4"]} />
            { c4check === 2 && 
              <input id='C4-secondary' type={"number"} defaultValue={20} min={10} max={72} ref={refList["c4-secondary"]} />
            }
            { c4check === 4 && 
              <input id='C4-secondary' type={"number"} defaultValue={50} min={20} max={60} ref={refList["c4-secondary"]} />
            }
            </span></span>
            <span><label htmlFor='C5'>C5 Parameter</label><input id='C5' type={"number"} defaultValue={10} min={0} max={10} ref={refList["c5"]} /></span>
            <span><label htmlFor='C11'>C11 Parameter</label><input id='C11' type={"number"} defaultValue={0} min={0} max={3} ref={refList["c11"]} /></span>
            <span><label htmlFor='C12'>C12 Parameter</label><input id='C12' type={"number"} defaultValue={4} min={0} max={7} ref={refList["c12"]} /></span>
            <span><label htmlFor='C13'>C13 Parameter</label><input id='C13' type={"number"} defaultValue={0} min={0} max={5} ref={refList["c13"]} /></span>
            <span><label htmlFor='C14'>C14 Parameter</label><input id='C14' type={"number"} defaultValue={1} min={1} max={3} ref={refList["c14"]} /></span>
          </form>
          <div>{ code?.join(" ") }</div>
        </section>
      </main>
    </div>
  );
}

export default App;
