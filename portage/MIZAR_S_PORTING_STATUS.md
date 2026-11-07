# Geeetech Mizar S — suivi de portage Marlin 2.1.2.9

Dernière mise à jour : 2026-11-07 (Europe/Paris)

## Cible et limites

- Base : MarlinFirmware/Marlin 2.1.2.9, commit 7d300b42acadb3e7467e9f4b4fc03ebea592c2d0.
- Branche de travail : mizar-s-marlin-2.1.2.9.
- Branche auditée avant ce checkpoint : dcdc689471599e2d8a87f876ef0005e3980ea266.
- Les branches main et bugfix-2.1.x n’ont pas été modifiées.

## État de compilation vérifié

Le workflow GitHub Actions « Mizar S Marlin 2.1.2.9 build » exécute `pio run -e STM32F103VE_GTM32_maple`.

- Run 42391026752, commit a2f10634a558782f043a20b1b609f06d00534151 : échec sur des appels LVGL ambigus et des positions de boutons non déclarées.
- Run 42391389321, commit 178320bb0305db298697b5b6800c628d78940535 : les erreurs précédentes sont corrigées; échec sur `style_yellow_label` absent.
- Run 42391546130, commit 83da497d40881ffc358fad2f91cde9777d51a2c5 : échec dans le dialogue de pause, réponses M600 optionnelles absentes.
- Run 42391772326, commit ccc022b013af2a1c2a2451a647d3c6c68d22319c : échec sur `manual_feedrate_mm_s.z` et un style jaune absent.
- Run 42391981474, commit c48792b6b82b4952561b6d4292234c8200a6ed5a : la vitesse Z et le premier style sont corrigés; un second `style_yellow_label` reste absent.
- Run 42392232781, commit 10addfc5f676c206cb0d49ef122f31ba3eb86e72 : échec sur `pause_menu_response` quand `M600_PURGE_MORE_RESUMABLE` est désactivé.
- Run 42392501067, commit dcdc689471599e2d8a87f876ef0005e3980ea266 : échec sur `BTN_ROW1_COL2_POS`, `BTN_ROW1_COL3_POS` et `BTN_ROW2_COL3_POS` dans `draw_tool.cpp`. Le compilateur a aussi signalé un littéral multi-caractère et un débordement dans le terminateur de `public_buf_l`; ce checkpoint corrige ces positions et utilise le vrai caractère nul.
- Les avertissements du framework Maple et la dépréciation de cet environnement restent présents. Aucun build complet réussi n’est démontré; le prochain run doit être examiné.

## Travail déjà intégré

- Carte GTM32/Mizar S et environnement STM32F103VE_GTM32_maple, avec script de relocation bootloader.
- Dual leveling ABL/MBL : objets et grilles distincts, sélection runtime, routage G-code, planificateur et persistance EEPROM V89.
- M421 MBL lit les index en 32 bits puis les valide avant l’accès au maillage.
- Écrans LVGL auto et manuel 4×4, sélection de mode, sauvegarde/restauration de maillage et retour/cancel en cours d’adaptation à l’API 2.1.2.9.
- Le dialogue de pause protège les réponses optionnelles avec M600_PURGE_MORE_RESUMABLE.

## Runout, PLR et ressources

- Configuration LVGL : MT_DET_1_PIN et FIL_RUNOUT_PIN utilisent PA15; le capteur natif Marlin reste désactivé pour éviter un double déclenchement avec le handler écran. Le chemin pause/reprise et le signal physique restent à vérifier.
- Chemin alternatif sans LVGL : PA4 est affecté au runout. Le conflit éventuel avec les fonctions écran doit être résolu avant d’utiliser cette variante.
- PLR : stockage EEPROM séparé, CRC, garde de taille et sauvegarde du Z portés en première passe; le parcours complet d’arrêt/reprise SD n’est pas validé.
- Les sources LVGL et en-têtes français/anglais sont présents. Le dépôt firmware ne contient pas les images MKS .bin chargées depuis F:/; la source constructeur fournit des JPG et des outils, mais le paquet d’images de l’écran reste à assembler et vérifier.
- La carte, runout/perte secteur, migration EEPROM et séquences d’impression n’ont pas été validés sur imprimante.

## À terminer

1. Obtenir une compilation PlatformIO réussie, puis traiter les erreurs révélées.
2. Auditer les séquences LVGL ABL/MBL : démarrage, annulation, arrêt, fin, sauvegarde, retour et cohérence du nivellement actif.
3. Auditer le runout avec M600, réinsertion, purge et reprise du fichier.
4. Auditer PLR : sauvegarde EEPROM, détection PE3, fichier SD, reprise et effacement.
5. Intégrer ou documenter précisément le paquet d’images TFT et vérifier les chaînes FR/EN.
6. Conserver une procédure de flash et migration EEPROM; effectuer la validation sur banc avant livraison matérielle.

## Règle de livraison

Ne pas présenter le firmware comme prêt à flasher ou validé matériellement sans build réussi, revue statique des chemins concernés et essais réels de la carte.