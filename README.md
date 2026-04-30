# Сборник микропроектов в Ардуино среде

## Нужно установить flatpak

```bash
sudo apt update && sudo apt install flatpak -y
```

## Добавление репозитория Flathub

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
```

После этого нужно перезагрузить систему, чтобы изменения вступили в силу

## Установка Arduino IDE на Linux

```bash
flatpak install flathub cc.arduino.IDE2
```

## Добавление пользователя в группу dialout

```bash
sudo usermod -a -G dialout $USER
```

## Предоставление расширенных прав Flatpak

```bash
flatpak override --user --device=all cc.arduino.IDE2
flatpak override --user --filesystem=host cc.arduino.IDE2
```

После выполнения всех этих команд нужно перезагрузить компьютер. 

Это важный шаг, чтобы применить изменения в группе dialout.
