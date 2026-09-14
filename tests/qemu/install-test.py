import subprocess
import time
import os
import socket
import json
import sys

def send_qmp_command(sock, cmd):
    sock.sendall((json.dumps(cmd) + '\n').encode())
    res = b''
    while True:
        chunk = sock.recv(4096)
        res += chunk
        if b'\n' in chunk:
            break
    return res.decode()

def send_keys(sock, text):
    for char in text:
        if char == ' ':
            send_qmp_command(sock, {"execute": "human-monitor-command", "arguments": {"command-line": "sendkey spc"}})
        elif char == '-':
            send_qmp_command(sock, {"execute": "human-monitor-command", "arguments": {"command-line": "sendkey minus"}})
        elif char == '/':
            send_qmp_command(sock, {"execute": "human-monitor-command", "arguments": {"command-line": "sendkey slash"}})
        elif char == '_':
            send_qmp_command(sock, {"execute": "human-monitor-command", "arguments": {"command-line": "sendkey shift-minus"}})
        elif char == '.':
            send_qmp_command(sock, {"execute": "human-monitor-command", "arguments": {"command-line": "sendkey dot"}})
        else:
            send_qmp_command(sock, {"execute": "human-monitor-command", "arguments": {"command-line": f"sendkey {char}"}})
        time.sleep(0.1)
    send_qmp_command(sock, {"execute": "human-monitor-command", "arguments": {"command-line": "sendkey ret"}})
    time.sleep(1)

def main():
    iso_path = '/home/reny/Documents/OS/build/rios-live-amd64.hybrid.iso'
    disk_path = '/tmp/rios-target-disk.qcow2'
    qmp_sock = '/tmp/qmp_iso_test.sock'
    
    if os.path.exists(disk_path):
        os.remove(disk_path)
    
    # Create a 20G virtual disk
    subprocess.run(['qemu-img', 'create', '-f', 'qcow2', disk_path, '20G'], check=True)

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

    print("[*] Launching QEMU with RiOS Live ISO...")
    proc = subprocess.Popen(cmd)
    time.sleep(2)

    try:
        sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        sock.connect(qmp_sock)
        sock.recv(4096)
        send_qmp_command(sock, {"execute": "qmp_capabilities"})
        print("[*] QMP connected.")

        # Boot default live entry
        time.sleep(2)
        send_qmp_command(sock, {"execute": "human-monitor-command", "arguments": {"command-line": "sendkey ret"}})
        print("[*] Sent ENTER key to boot Live environment.")

        print("[*] Waiting 60 seconds for live boot...")
        for i in range(12):
            time.sleep(5)
            sys.stdout.write('.')
            sys.stdout.flush()
        print("")

        # Switch to TTY3 (ctrl-alt-f3)
        send_qmp_command(sock, {"execute": "human-monitor-command", "arguments": {"command-line": "sendkey ctrl-alt-f3"}})
        time.sleep(5)
        
        # Login
        send_keys(sock, "ratana")
        time.sleep(2)
        send_keys(sock, "ratana")
        time.sleep(5)

        print("[*] Running installation command via TTY3...")
        send_keys(sock, "sudo ri-installer --target /dev/vda -y > /tmp/install.log 2>&1")
        
        # Installation might take a few minutes
        print("[*] Waiting 4 minutes for installation to finish...")
        for i in range(24):
            time.sleep(10)
            sys.stdout.write('.')
            sys.stdout.flush()
        print("")
        
        print("[*] Shutting down Live system...")
        send_keys(sock, "sudo poweroff")
        time.sleep(20)

        sock.close()
    finally:
        print("[*] Terminating QEMU live process...")
        try:
            proc.terminate()
            proc.wait(timeout=5)
        except:
            proc.kill()

    # Now boot from the installed disk
    cmd2 = [
        'qemu-system-x86_64',
        '-enable-kvm',
        '-m', '4096',
        '-smp', '2',
        '-drive', f'file={disk_path},format=qcow2,if=virtio',
        '-vga', 'std',
        '-vnc', ':98',
        '-qmp', f'unix:{qmp_sock},server,nowait'
    ]
    
    if os.path.exists(qmp_sock):
        os.remove(qmp_sock)

    print("[*] Launching QEMU with newly installed OS on virtual disk...")
    proc2 = subprocess.Popen(cmd2)
    time.sleep(2)

    try:
        sock2 = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        sock2.connect(qmp_sock)
        sock2.recv(4096)
        send_qmp_command(sock2, {"execute": "qmp_capabilities"})
        print("[*] Waiting 60 seconds for installed OS to boot...")
        for i in range(12):
            time.sleep(5)
            sys.stdout.write('.')
            sys.stdout.flush()
        print("")
        
        # Capture screenshot
        send_qmp_command(sock2, {"execute": "human-monitor-command", "arguments": {"command-line": "screendump /tmp/installed_desktop.ppm"}})
        print("[+] Captured installed desktop screen.")
    finally:
        print("[*] Terminating QEMU installed OS process...")
        try:
            proc2.terminate()
            proc2.wait(timeout=5)
        except:
            proc2.kill()
    
    artifact_dir = '/home/reny/.gemini/antigravity/brain/8eabf34f-253b-46fe-aaeb-da6e420486c1'
    png = f'{artifact_dir}/installed_desktop.png'
    if os.path.exists('/tmp/installed_desktop.ppm'):
        subprocess.run(['ffmpeg', '-y', '-i', '/tmp/installed_desktop.ppm', png], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if os.path.exists(png):
            print(f"[✔] Screenshot saved: {png}")
            return True
    return False

if __name__ == '__main__':
    main()
