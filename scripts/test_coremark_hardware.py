import urllib.request, json, time, sys, threading, serial

def call_rpc(method, params=None):
    payload = {'jsonrpc': '2.0', 'method': method, 'id': 1}
    if params:
        payload['params'] = params
    req = urllib.request.Request(
        'http://localhost:8088/rpc',
        data=json.dumps(payload).encode('utf-8'),
        headers={'Content-Type': 'application/json'}
    )
    with urllib.request.urlopen(req) as resp:
        return json.loads(resp.read().decode('utf-8'))

def test_coremark():
    launch_name = "CoreMark_S32K144W_Debug_FLASH_PNE"
    
    # Serial capture setup
    serial_data = []
    stop_serial = threading.Event()
    
    def serial_reader():
        try:
            with serial.Serial('COM5', 115200, timeout=0.2) as ser:
                ser.reset_input_buffer()
                while not stop_serial.is_set():
                    data = ser.read(ser.in_waiting or 1)
                    if data:
                        serial_data.append(data.decode('utf-8', errors='replace'))
        except Exception as e:
            print(f"[Serial thread warning] {e}")

    ser_thread = threading.Thread(target=serial_reader, daemon=True)
    ser_thread.start()

    print(f"1. Launching {launch_name}...")
    res = call_rpc('startDebug', {'name': launch_name, 'build': False})
    assert 'error' not in res, f"startDebug failed: {res}"

    time.sleep(8)

    print("2. Setting breakpoint at portable_fini...")
    bp = call_rpc('runMiDebugCommand', {'launch': launch_name, 'command': 'break portable_fini'})
    assert 'error' not in bp, f"Failed to set breakpoint: {bp}"

    print("3. Resuming execution...")
    cont = call_rpc('runMiDebugCommand', {'launch': launch_name, 'command': 'continue'})
    assert 'error' not in cont, f"Failed to continue: {cont}"

    print("4. Waiting for benchmark to complete...")
    stopped = False
    for i in range(15):
        time.sleep(2)
        status = call_rpc('runMiDebugCommand', {'launch': launch_name, 'command': 'info program'})
        if 'it stopped' in str(status).lower():
            stopped = True
            print(f"Target stopped at portable_fini after {(i+1)*2}s.")
            break

    assert stopped, "Target did not halt at portable_fini within timeout!"

    print("5. Extracting CoreMark output buffer via GDB...")
    buf = call_rpc('runMiDebugCommand', {'launch': launch_name, 'command': 'printf "%s\\n", g_coremark_output_buffer'})
    output = buf.get('result', {}).get('result', '')
    print("\n--- CoreMark GDB Buffer Report ---")
    print(output)

    assert "Correct operation validated" in output, "Validation failed!"
    assert "CoreMark 1.0 :" in output, "Score missing!"
    assert "Iterations       : 1200" in output, f"Iterations mismatch: {output}"

    # Stop serial reading and verify COM5 captured stream
    time.sleep(1)
    stop_serial.set()
    ser_thread.join(timeout=2)
    captured_serial = "".join(serial_data)
    print("\n--- Real Hardware COM5 Serial Stream ---")
    print(captured_serial)

    assert "EEMBC CoreMark 1.0" in captured_serial, "COM5 failed to receive CoreMark banner!"
    assert "Correct operation validated" in captured_serial, "COM5 failed to receive validation!"

    print("6. Cleaning up debug session...")
    call_rpc('terminateDebug', {'launch': launch_name})
    print("CoreMark hardware & COM5 serial verification PASSED!")

if __name__ == '__main__':
    test_coremark()
