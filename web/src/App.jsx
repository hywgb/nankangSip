import React, { useState } from 'react'

export default function App() {
  const [dst, setDst] = useState('')
  const [log, setLog] = useState([])

  const append = (m) => setLog((prev) => [...prev, `[${new Date().toLocaleTimeString()}] ${m}`])

  async function health() {
    try {
      const res = await fetch('/api/health')
      append(`health: ${res.status}`)
    } catch (e) {
      append(`health error: ${e}`)
    }
  }

  async function makeCall() {
    append(`makeCall -> ${dst}`)
    // 占位：实际应调用 /call/make 并通过 WSS 订阅事件
  }
  function hangup() { append('hangup (stub)') }
  function hold() { append('hold (stub)') }
  function unhold() { append('unhold (stub)') }

  return (
    <div style={{fontFamily:'system-ui',maxWidth:680,margin:'40px auto'}}>
      <h2>ITACATI 坐席工作台（骨架）</h2>
      <div style={{display:'flex',gap:8,alignItems:'center'}}>
        <input placeholder="目标号码/URI" value={dst} onChange={e=>setDst(e.target.value)} />
        <button onClick={makeCall}>外呼</button>
        <button onClick={hangup}>挂断</button>
        <button onClick={hold}>保持</button>
        <button onClick={unhold}>恢复</button>
        <button onClick={health}>健康检查</button>
      </div>
      <pre style={{background:'#111',color:'#0f0',padding:12,marginTop:16,height:240,overflow:'auto'}}>
        {log.join('\n')}
      </pre>
      <p style={{opacity:.7}}>提示：当前为前后端骨架，媒体/信令将在后续里程碑逐步接入。</p>
    </div>
  )
}