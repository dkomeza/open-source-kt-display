import React, { useEffect, useRef, useState } from 'react';
import './scss/style.scss'

function App() {
  const [code, setCode] = useState<number[] | null>([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
  const [c4check, setC4check] = useState<number>(0)
  const [copied, setCopied] = useState(false)
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

  function copyCode(e: any) {
    navigator.clipboard.writeText(e.target.innerText);
    setCopied(true);
    window.setTimeout(() => {
      setCopied(false);
    }, 2000)
  }
  
  function handleInput() {
    setC4check(Number(refList["c4"].current?.value))
    let b2b4Temp = []
    let tempCode = []

    let speedTemp = (Number(refList["speed"].current?.value) - 10).toString(2)
    let wheelSizeTemp = Number(refList["wheel-size"].current?.value).toString(2)
    let p2Temp = Number(refList["p2"].current?.value)
    let p3Temp = (Number(refList["p3"].current?.value))
    let p4Temp = (Number(refList["p4"].current?.value))
    let pTemps = (p2Temp | (p3Temp << 3) | (p4Temp << 4)).toString(2)

    

    speedTemp = fillZeroes(speedTemp, 6)
    wheelSizeTemp = fillZeroes(wheelSizeTemp, 5)
    pTemps = fillZeroes(pTemps, 5)

    for (let i = 0; i < 5; i++) {
      b2b4Temp.push(speedTemp[i + 1])
    }
    console.log(speedTemp)
    console.log(b2b4Temp)
    for (let i = 0; i < 5; i++) {
      b2b4Temp.push(wheelSizeTemp[i])
    }
    b2b4Temp[10] = speedTemp[0]
    b2b4Temp[11] = pTemps[0]
    b2b4Temp[12] = pTemps[1]
    b2b4Temp[13] = pTemps[2]
    b2b4Temp[14] = pTemps[3]
    b2b4Temp[15] = pTemps[4]
    let b2Temp = b2b4Temp.slice(0, 8).join("")
    let b4Temp = b2b4Temp.slice(8, 16).join("")

    tempCode[0] = parseInt(refList["p5"].current?.value || "0")
    tempCode[1] = 1 // Handled by the display
    tempCode[2] = parseInt(b2Temp, 2)
    tempCode[3] = Number(refList["p1"].current?.value)
    tempCode[4] = parseInt(b4Temp, 2)
    tempCode[5] = 0
    tempCode[6] = ((Number(refList["c1"].current?.value)) << 3) | Number(refList["c2"].current?.value)
    tempCode[7] = ((Number(refList["c14"].current?.value)) << 5) | Number(refList["c5"].current?.value) | 128
    tempCode[8] = ((Number(refList["c4"].current?.value)) << 5) | Number(refList["c12"].current?.value)
    tempCode[9] = Number(refList["c4-secondary"].current?.value) ? Number(refList["c4-secondary"].current?.value) : 20  
    tempCode[10] = ((Number(refList["c13"].current?.value)) << 2) | 1
    tempCode[11] = 50
    tempCode[12] = 14

    let crcTemp = 0

    for (let i = 0; i < tempCode.length; i++) {
      if (i !== 5) {
        crcTemp ^= tempCode[i]
      }
    }
    crcTemp ^= 3
    
    tempCode[5] = crcTemp
    setCode(tempCode)
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
            <span><label htmlFor='wheel-size'>Wheel Size</label><select id='wheel-size' ref={refList["wheel-size"]} defaultValue={20} >
              <option value={22}>05"</option>
              <option value={18}>06"</option>
              <option value={10}>08"</option>
              <option value={14}>10"</option>
              <option value={2}>12"</option>
              <option value={6}>14"</option>
              <option value={0}>16"</option>
              <option value={4}>18"</option>
              <option value={8}>20"</option>
              <option value={12}>23"</option>
              <option value={16}>24"</option>
              <option value={20}>26"</option>
              <option value={24}>700C</option>
              <option value={24}>27.5"</option>
              <option value={28}>28"</option>
              <option value={30}>29"</option>
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
          <div className='code' onClick={(e) => copyCode(e)}>{ code?.join(", ") }
          
          </div>
          {copied && 
            <span className='copied'>Code copied to clipboard</span>
          }
        </section>
      </main>
    </div>
  );
}

export default App;
