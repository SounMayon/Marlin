# Audit des ressources TFT Mizar S

Audit mis à jour le 2026-11-07 après comparaison du dépôt Geeetech, du paquet firmware fourni et des ressources MKS réellement utilisées par Marlin 2.1.2.9.

## Révisions et sources comparées

- Dépôt constructeur : [Geeetech3D/Mizar-S à d3656d6db4bb89f552f9afd3aec013db02a1cff9](https://github.com/Geeetech3D/Mizar-S/tree/d3656d6db4bb89f552f9afd3aec013db02a1cff9).
- Code du port : [SounMayon/Marlin, révision d81c6016694c6b01754f524dde5e877e401e144f](https://github.com/SounMayon/Marlin/tree/d81c6016694c6b01754f524dde5e877e401e144f).
- L'outil Marlin 2.1.2.9 [télécharge les ressources MKS depuis la révision 0263cdaccf](https://github.com/MarlinFirmware/Marlin/blob/2.1.2.9/buildroot/share/PlatformIO/scripts/download_mks_assets.py). Il copie Firmware/mks_pic et Firmware/mks_font dans le dossier assets du build.
- Le fabricant MKS publie ces répertoires dans [Firmware/mks_pic](https://github.com/makerbase-mks/Mks-Robin-Nano-Marlin2.0-Firmware/tree/0263cdaccf/Firmware/mks_pic) et [Firmware/mks_font](https://github.com/makerbase-mks/Mks-Robin-Nano-Marlin2.0-Firmware/tree/0263cdaccf/Firmware/mks_font).

Le Mizar S emploie son UI constructeur dans le dépôt Geeetech; le port utilise l'UI LVGL de Marlin/MKS. Les noms d'images ne sont donc pas identiques.

## Paquet constructeur fourni

L'archive MizarS_FM_V201A_230512.rar contient le journal de versions V2.0.1 du 12 mai 2023, un ancien GTM32Source.bin, 107 images assets/*.bin et FontUNIGBK.bin. Le binaire firmware n'a pas été utilisé.

Comparaison par nom exact des fichiers :

| Jeu de ressources | Images | Police | Noms d'images du port présents |
| --- | ---: | ---: | ---: |
| Archive Mizar S fournie | 107 | 1 | 61 / 120 |
| Paquet MKS épinglé par Marlin 2.1.2.9 | 139 | 1 | 120 / 120, sans tenir compte de la casse |

Le paquet Geeetech est celui de l'UI constructeur. Ses 59 noms d'images absents parmi les 120 noms du port ne sont pas des fichiers manquants du paquet MKS utilisé pour ce port. Les binaires du paquet Geeetech ne sont pas copiés dans le dépôt.

## Vérification du paquet utilisé par le port

La liste Firmware/mks_pic à la révision 0263cdaccf contient 139 images (4 348 174 octets); Firmware/mks_font fournit FontUNIGBK.bin (470 252 octets). Les 120 noms distincts d'images attendus par pic_manager.cpp sont présents dans ce paquet en comparant les noms sans distinction de casse. Un seul nom varie par la casse : le code demande bmp_mov_changespeed.bin et MKS fournit bmp_mov_changeSpeed.bin. Le gestionnaire d'images compare également les noms sans distinction de casse.

Le workflow de compilation vérifie maintenant que le dossier généré existe et que chacun des noms F(...) non commentés dans pic_manager.cpp est présent, sans tenir compte de la casse. Il a vérifié 118 noms lors du run 42400737794.

L'artefact de ce run contient maintenant firmware.bin, firmware.elf et le dossier assets généré. Le workflow de mise à jour exige donc moins de préparation manuelle du paquet de ressources. Le succès du build et de ce contrôle ne vérifie pas le rendu sur l'écran réel.

## Langues

Les quatre images attendues pour le choix français/anglais — bmp_english.bin, bmp_english_sel.bin, bmp_french.bin et bmp_french_sel.bin — existent dans le paquet MKS épinglé. L'archive Geeetech fournie ne contient que bmp_english_sel.bin parmi ces quatre images. Les chaînes françaises et anglaises du port, ainsi que les fichiers nécessaires à l'écran de choix, sont donc présents dans les ressources livrées par le build.

## Format et mise à jour

Le README du dépôt MKS indique une sortie Binary RGB565 pour les images LVGL et la copie du dossier assets sur la carte SD/TF. Le [README Geeetech](https://github.com/Geeetech3D/Mizar-S/blob/d3656d6db4bb89f552f9afd3aec013db02a1cff9/README.md) documente aussi Bin_565 pour son convertisseur. Le [Wiki Geeetech](https://wiki.geeetech.com/index.php/Geeetech_Mizar_S_3D_Printer#Update_Firmware) indique de mettre le binaire firmware et assets à la racine de la carte TF, puis de les supprimer après mise à jour et de refaire le nivellement.

Le fichier firmware GTM32Source.bin de l'archive date du paquet V2.0.1 de 2023; il n'est pas le build du port 2.1.2.9 et ne doit pas être présenté comme tel.

## Verdict et contrôles restants

Le paquet TFT est maintenant récupéré à une révision fixe, contrôlé dans le workflow, et inclus avec le firmware dans l'artefact CI. Le jeu d'images constructeur joint n'est pas utilisé comme substitut aux images MKS de l'UI du port.

À vérifier encore sur l'écran Mizar S : mise à jour TFT, rendu des icônes, choix FR/EN et écrans ABL/MBL. Aucun de ces comportements n'a été testé sur la machine.