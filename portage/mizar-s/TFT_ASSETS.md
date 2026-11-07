# Audit des ressources TFT Mizar S

Audit comparatif effectué le 2026-11-07 sur les révisions indiquées ci-dessous. Cette note corrige le comptage des ressources publié précédemment.

## Révisions comparées

- Sources constructeur : [Geeetech3D/Mizar-S, main à d3656d6db4bb89f552f9afd3aec013db02a1cff9](https://github.com/Geeetech3D/Mizar-S/tree/d3656d6db4bb89f552f9afd3aec013db02a1cff9).
- Port Marlin : [SounMayon/Marlin, mizar-s-marlin-2.1.2.9 à d81c6016694c6b01754f524dde5e877e401e144f](https://github.com/SounMayon/Marlin/tree/d81c6016694c6b01754f524dde5e877e401e144f).
- Le dépôt constructeur fournit le firmware Marlin 2.0.7 pour l’écran Mizar S d’origine. Le port utilise le MKS UI de Marlin 2.1.2.9, dont les références d’images ne sont pas identiques.

## Comptage vérifié

| Élément | Résultat |
| --- | ---: |
| JPG dans `Geeetech3D/Mizar-S/Resource/icon` | 107 |
| Références d’images dans le `pic_manager.cpp` constructeur | 107 sur 107 ont un JPG de même nom |
| Noms d’images distincts demandés par le `pic_manager.cpp` du port | 120 |
| Noms du port avec un JPG constructeur de même nom | 61 |
| Noms d’images du port sans JPG constructeur homonyme | 59 |

Le gestionnaire d’origine référence aussi `FontUNIGBK.bin`, une ressource de police séparée des 107 icônes JPG. Le gestionnaire du port référence lui aussi cette police; elle ne se trouve pas comme image dans `Resource/icon`.

Le chemin français/anglais du port appelle directement `bmp_english.bin`, `bmp_english_sel.bin`, `bmp_french.bin` et `bmp_french_sel.bin`. Le dépôt constructeur contient `bmp_english_sel.jpg`, mais pas les trois autres noms. Les libellés textuels français/anglais sont donc présents dans le code, mais l’écran de choix des langues dépend encore d’images qui ne sont pas fournies par ce dépôt.

Le dépôt du port ne contient aucun paquet TFT `assets/` ni icône MKS `.bin`. Le seul fichier `.bin` trouvé dans l’arbre est un binaire de bootloader JGAurora sans rapport avec les images du Mizar S.

## Format et procédure constructeur

Le [README constructeur](https://github.com/Geeetech3D/Mizar-S/blob/d3656d6db4bb89f552f9afd3aec013db02a1cff9/README.md) donne ces réglages pour son convertisseur LVGL :

- `Transparency=NONE-TrueColor`
- `lv_img_dsc_t=` vide
- sortie `Bin_565`

Le [README générique de l’outil](https://github.com/Geeetech3D/Mizar-S/blob/d3656d6db4bb89f552f9afd3aec013db02a1cff9/Resource/Lvgl_image_convert_tool/Readme.md) précise que son auteur n’avait pas utilisé la sortie `.bin`. Cette remarque est générique; elle ne remplace pas le réglage Bin_565 donné dans le README du Mizar S. Le décodeur et les fichiers du paquet final restent à vérifier avec l’écran réel.

La [procédure du Wiki Geeetech](https://wiki.geeetech.com/index.php/Geeetech_Mizar_S_3D_Printer#Update_Firmware) demande de placer le binaire firmware et le dossier `assets` à la racine de la carte TF, puis de supprimer les deux après la mise à jour et de refaire le nivellement.

## Verdict

Les JPG du dépôt constructeur suffisent pour reproduire les 107 noms de son propre UI Marlin 2.0.7, mais pas pour satisfaire par nom le gestionnaire MKS conservé dans le port 2.1.2.9. Les 59 références sans JPG homonyme, les images de langue manquantes et l’absence d’un paquet `assets/` empêchent de valider l’intégration écran.

Le lien [Geeetech Support download 57](https://support.geeetech.com/download.html?download_id=57) et le [Pastebin fourni](https://pastebin.com/Uz5YQZjj) n’étaient pas accessibles pendant cet audit. Leur contenu n’a donc pas été utilisé pour supposer que ces ressources manquantes existent.

### À faire avant de déclarer l’écran intégré

1. Récupérer et inspecter le paquet officiel de téléchargement, s’il fournit le dossier `assets/` pour la bonne révision d’écran.
2. Pour chacun des 120 noms du port, identifier une image correcte ou retirer/remapper proprement la référence non utilisée; ne pas créer des alias visuels arbitraires.
3. Convertir les images en Bin_565 avec les noms attendus et constituer le dossier d’update.
4. Vérifier sur l’écran la mise à jour TFT, les icônes, le choix FR/EN et les vues de nivellement ABL/MBL.
