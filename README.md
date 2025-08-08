# ITACATI (pjsip + WebRTC + React) — 架构与开发指南

## 概述
ITACATI 是一个基于 pjsip 的软电话/坐席工作台骨架，前端使用 React（Vite），后端使用 C++17。目标：
- 浏览器端用 WebRTC 音视频；后端通过 WebRTC↔SIP 网关桥接到 pjsip（SIP/RTP）。
- 提供通话控制 API（呼入/外呼、保持/恢复、转接/三方、DTMF、录音）。
- 可观测与可运维，后续接入 Prometheus / OpenTelemetry / Homer。

本仓库当前为“可编译/可运行的骨架”，不包含 pjsip/webrtc 实现细节，便于逐步落地。

## 目录
- `backend/` C++17 服务：
  - 计划模块：
    - `sip/` pjsip 封装（SIP/SDP/RTP/ICE/SRTP）。
    - `webrtc/` WebRTC-SIP 网关桥接（与浏览器 DTLS-SRTP/ICE 终止，和 sip 桥接 RTP）。
    - `api/` 控制面 API（先占位，后续 gRPC/WebSocket/REST）。
  - 当前仅提供 `main.cpp` 启动骨架与模块占位头文件。
- `web/` React 前端（Vite）：基础拨号 UI、设备检测占位、API 调用示例。
- `api/openapi.yaml` 控制面 API 合同草案（后续与实现保持一致）。

## 后端（C++17）
### 依赖
- CMake ≥ 3.16
- C++17 编译器（gcc/clang/msvc）

> 后续会加入：pjsip、libsrtp2、libwebrtc/mediasoup C++、Redis/PostgreSQL 客户端、OpenTelemetry、Prometheus-C++ 等。

### 构建
```bash
cd backend
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/itacati_server
```

启动后会打印占位日志，表示进程存活。实际 SIP/WebRTC/HTTP 功能将按里程碑逐步实现。

## 前端（React + Vite）
### 依赖
- Node.js 18+

### 运行
```bash
cd web
npm i
npm run dev
```
浏览器访问 Vite 提示的本地地址。当前 UI 提供拨号盘与健康检查调用示例。

## API 合同
- 参见 `api/openapi.yaml`
  - `/api/health` 健康检查（示例）
  - `/webrtc/offer` 与 `/webrtc/answer`：WebRTC SDP 交换（占位）
  - `/call/*`：外呼/挂断/保持/恢复/转接/DTMF 等（占位）

## 里程碑（建议）
- M1：pjsip 注册/基本呼叫/保持/转接/DTMF，TURN 集群；前端完成 WebRTC SDP 交换。
- M2：WebRTC↔SIP 桥接、旁路录音、可观测指标；错误恢复。
- M3：三方会议、合规（录音加密/留存/审计）、灰度与演练、性能优化。

## 许可证
内部开发样例骨架。后续按项目策略添加 LICENSE。
