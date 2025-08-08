import React, { useEffect, useRef, useState } from 'react'

export default function App() {
  const [dst, setDst] = useState('10086')
  const [log, setLog] = useState([])
  const sinceRef = useRef(0)
  const timerRef = useRef(null)

  const append = (m) => setLog((prev) => [...prev, `[${new Date().toLocaleTimeString()}] ${m}`])

  async function health() {
    try { const res = await fetch('/api/health'); append(`health: ${res.status}`) } catch (e) { append(`health error: ${e}`) }
  }

  async function makeCall() {
    try {
      const res = await fetch('/call/make', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({dst}) })
      const j = await res.json(); append(`makeCall -> callId=${j.callId}`)
    } catch (e) { append(`makeCall error: ${e}`) }
  }
  async function hangup() {
    const id = prompt('callId?')
    if (!id) return
    const res = await fetch('/call/hangup', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({callId:id}) })
    append(`hangup -> ${res.status}`)
  }
  async function hold() {
    const id = prompt('callId?'); if (!id) return
    const res = await fetch('/call/hold', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({callId:id}) })
    append(`hold -> ${res.status}`)
  }
  async function unhold() {
    const id = prompt('callId?'); if (!id) return
    const res = await fetch('/call/unhold', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({callId:id}) })
    append(`unhold -> ${res.status}`)
  }

  async function poll() {
    try {
      const res = await fetch(`/events/poll?since=${sinceRef.current}`)
      const arr = await res.json()
      if (Array.isArray(arr)) {
        for (const e of arr) {
          append(`event ${e.id} ${e.type} callId=${e.callId}`)
          sinceRef.current = Math.max(sinceRef.current, e.id)
        }
      }
    } catch(e) { /* ignore */ }
  }

  useEffect(()=>{
    timerRef.current = setInterval(poll, 800)
    return ()=> clearInterval(timerRef.current)
  },[])

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
      <pre style={{background:'#111',color:'#0f0',padding:12,marginTop:16,height:300,overflow:'auto'}}>
        {log.join('\n')}
      </pre>
      <p style={{opacity:.7}}>当前后端使用内存 CallManager 模拟，后续替换为 pjsip + webrtc 桥接。</p>
    </div>
  )
}