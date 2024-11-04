# g4batstat-tray
GTK Battery Status for PowerBook G4

I didn't write this, I just coaxed an LLM to write this for me. It's probably broken and I don't know how to fix it.


![tinyscreenshot](https://github.com/user-attachments/assets/7f9a5199-82f7-4419-8804-a780f6d1f1be)


![screenshot2](https://github.com/user-attachments/assets/1f8de068-f28e-461e-99de-35980f106c80)


This was the compiler command. I built this on ArchLinuxPOWER. 

gcc -o g4batstat-tray g4batstat-tray.c $(pkg-config --cflags --libs gtk+-3.0)

