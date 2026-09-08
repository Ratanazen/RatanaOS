import subprocess
import time
import os
import socket
import json

def send_qmp_command(sock, cmd):
    sock.sendall((json.dumps(cmd) + '\n').encode())
    res = b''
    while True:
        chunk = sock.recv(4096)
        res += chunk
        if b'\n' in chunk:
            break
    return res.decode()

def test_themes():
    qmp_sock_path = '/tmp/qmp_icon_test.sock'
    if os.path.exists(qmp_sock_path):
        os.remove(qmp_sock_path)

    qemu_cmd = [
        'qemu-system-x86_64',
        '-kernel', 'build/ratanaos32.bin',
        '-serial', 'pipe:/tmp/qemu_serial',
        '-vga', 'std',
        '-vnc', ':99',
        '-qmp', f'unix:{qmp_sock_path},server,nowait'
    ]

    for p in ['/tmp/qemu_serial.in', '/tmp/qemu_serial.out']:
        if os.path.exists(p):
            os.remove(p)
        os.mkfifo(p)

    proc = subprocess.Popen(qemu_cmd)
    time.sleep(1.0)

    try:
        sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        sock.connect(qmp_sock_path)
        sock.recv(4096)
        send_qmp_command(sock, {"execute": "qmp_capabilities"})

        with open('/tmp/qemu_serial.in', 'w') as f:
            f.write("gui\n")
            f.flush()

        time.sleep(1.5)

        # 1. WhiteSur Screenshot
        send_qmp_command(sock, {"execute": "human-monitor-command", "arguments": {"command-line": "screendump /tmp/whitesur_desktop.ppm"}})
        time.sleep(0.5)

        # 2. Switch theme: send 't' key
        send_qmp_command(sock, {"execute": "human-monitor-command", "arguments": {"command-line": "sendkey t"}})
        time.sleep(1.0)

        # 3. MacTahoe Screenshot
        send_qmp_command(sock, {"execute": "human-monitor-command", "arguments": {"command-line": "screendump /tmp/mactahoe_desktop.ppm"}})
        time.sleep(0.5)

        # 4. Switch theme: send 't' key again (Vector)
        send_qmp_command(sock, {"execute": "human-monitor-command", "arguments": {"command-line": "sendkey t"}})
        time.sleep(1.0)

        # 5. Vector Screenshot
        send_qmp_command(sock, {"execute": "human-monitor-command", "arguments": {"command-line": "screendump /tmp/vector_desktop.ppm"}})
        time.sleep(0.5)

        sock.close()
    finally:
        proc.terminate()
        proc.wait()

    os.system("ffmpeg -y -i /tmp/whitesur_desktop.ppm /tmp/whitesur_desktop.png 2>/dev/null")
    os.system("ffmpeg -y -i /tmp/mactahoe_desktop.ppm /tmp/mactahoe_desktop.png 2>/dev/null")
    os.system("ffmpeg -y -i /tmp/vector_desktop.ppm /tmp/vector_desktop.png 2>/dev/null")

    print("Screenshots captured successfully:")
    print(" - /tmp/whitesur_desktop.png")
    print(" - /tmp/mactahoe_desktop.png")
    print(" - /tmp/vector_desktop.png")

if __name__ == '__main__':
    test_themes()
