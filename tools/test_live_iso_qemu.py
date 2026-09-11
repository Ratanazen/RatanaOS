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

def main():
    iso_path = '/home/reny/Documents/OS/build/ratanaos-live-amd64.hybrid.iso'
    disk_path = '/tmp/ratanaos-target-disk.qcow2'
    qmp_sock = '/tmp/qmp_iso_test.sock'
    artifact_dir = '/home/reny/.gemini/antigravity/brain/8eabf34f-253b-46fe-aaeb-da6e420486c1'

    if os.path.exists(qmp_sock):
        os.remove(qmp_sock)

    cmd = [
        'qemu-system-x86_64',
        '-enable-kvm',
        '-m', '4096',
        '-smp', '2',
        '-cdrom', iso_path,
        '-drive', f'file={disk_path},format=qcow2,if=virtio',
        '-boot', 'd',
        '-vga', 'std',
        '-vnc', ':98',
        '-qmp', f'unix:{qmp_sock},server,nowait',
        '-no-reboot'
    ]

    print("[*] Launching QEMU with RatanaOS Live ISO...")
    proc = subprocess.Popen(cmd)
    time.sleep(2)

    try:
        sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        sock.connect(qmp_sock)
        sock.recv(4096)
        send_qmp_command(sock, {"execute": "qmp_capabilities"})
        print("[*] QMP connected.")

        # Send Enter to boot default live entry immediately
        time.sleep(2)
        send_qmp_command(sock, {"execute": "human-monitor-command", "arguments": {"command-line": "sendkey ret"}})
        print("[*] Sent ENTER key to boot Live environment.")

        # Wait 45s for full desktop / LightDM greeter
        print("[*] Waiting 45 seconds for desktop initialization...")
        time.sleep(45)

        send_qmp_command(sock, {"execute": "human-monitor-command", "arguments": {"command-line": "screendump /tmp/ratana_desktop_active.ppm"}})
        print("[+] Captured active desktop/greeter screen.")

        sock.close()
    finally:
        print("[*] Terminating QEMU test process...")
        proc.terminate()
        try:
            proc.wait(timeout=5)
        except:
            proc.kill()

    ppm = '/tmp/ratana_desktop_active.ppm'
    png = f'{artifact_dir}/ratana_desktop_active.png'
    if os.path.exists(ppm):
        subprocess.run(['ffmpeg', '-y', '-i', ppm, png], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if os.path.exists(png):
            print(f"[✔] Screenshot saved: {png}")

if __name__ == '__main__':
    main()
