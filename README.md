# USB Sound Notifier

`USB Sound Notifier` is a lightweight Windows background application that listens for USB device insertion events and plays a custom `.wav` sound when a device is detected.

It works at the USB device-interface level, so it can react to more than just removable drives. For example, phones, mice, keyboards, and USB flash drives may all trigger the notification.

## Features

- Runs as a background Windows application with no console window
- Detects USB device arrival events
- Plays a custom WAV notification sound
- Supports relative sound file paths for easy deployment
- Simple configuration through `config.ini`

## Project Files

- `main.cpp`: application source code
- `config.ini`: runtime configuration file
- `usb.WAV`: default notification sound
- `.vscode/tasks.json`: VS Code build task example
- `USBSoundNotifier.exe`: compiled executable, if already built

## Requirements

- Windows
- A C++ compiler that can build Win32 applications
- VS Code and MinGW-w64 / MSYS2 `g++` if you want to use the included build task

## Build

### Option 1: Build with the included VS Code task

The included task uses this compiler path:

```text
C:\msys64\ucrt64\bin\g++.exe
```

If your compiler is installed in a different location, update `.vscode/tasks.json` before building.

Then in VS Code:

1. Open the project folder.
2. Open `main.cpp`.
3. Run the default build task.
4. The output file will be created as `USBSoundNotifier.exe` in the project root.

### Option 2: Build manually from the terminal

Example command:

```powershell
C:\msys64\ucrt64\bin\g++.exe -g main.cpp -o USBSoundNotifier.exe -DUNICODE -D_UNICODE -lwinmm -mwindows
```

## Deployment

To deploy this project to another Windows machine, copy the following files into the same folder:

- `USBSoundNotifier.exe`
- `config.ini`
- `usb.WAV` or your custom WAV file

Recommended deployment structure:

```text
USBSoundNotifier/
|-- USBSoundNotifier.exe
|-- config.ini
`-- usb.WAV
```

Important:

- Keep `config.ini` in the same directory as `USBSoundNotifier.exe`
- Use a `.wav` file for the notification sound
- Relative paths in `config.ini` are resolved from the executable folder

## Configuration

Edit `config.ini`:

```ini
[Settings]
SoundFile=usb.WAV
```

You can set:

- A relative path, such as `usb.WAV` or `sounds\plug.wav`
- An absolute Windows path, if you prefer

Examples:

```ini
[Settings]
SoundFile=usb.WAV
```

```ini
[Settings]
SoundFile=sounds\plug.wav
```

```ini
[Settings]
SoundFile=C:\Alerts\usb.wav
```

## How to Use

1. Build the project or use the provided executable.
2. Make sure `config.ini` points to a valid `.wav` file.
3. Run `USBSoundNotifier.exe`.
4. Insert a USB device.
5. The application will play the configured sound when the device is detected.

The program runs as a message-only Windows application, so it does not open a normal window.

## Notes

- The app applies a short cooldown to avoid playing the sound repeatedly for the same insertion sequence.
- Detection is based on USB device-interface notifications, not just drive-letter changes.
- If no valid sound file is found, Windows may silently fail to play the sound.

## Troubleshooting

### No sound is played

- Confirm the path in `config.ini` is correct
- Confirm the target file is a valid `.wav`
- Make sure the sound file is accessible from the deployed machine

### The app does not detect a device

- Test with a different USB device
- Confirm the app is running
- Rebuild the executable if the current binary is outdated

## License

Add your preferred license information here if you plan to share the project publicly.
