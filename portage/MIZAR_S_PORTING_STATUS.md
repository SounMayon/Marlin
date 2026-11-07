# Geeetech Mizar S — suivi de portage Marlin 2.1.2.9

Dernière mise à jour : 2026-11-07 (Europe/Paris)

## Cible et limites

- Base : MarlinFirmware/Marlin 2.1.2.9, commit 7d300b42acadb3e7467e9f4b4fc03ebea592c2d0.
- Branche de travail : mizar-s-marlin-2.1.2.9.
- Branche auditée avant ce checkpoint : 58a006e4cef499c53757f1ed349218d530fe7479.
- Les branches main et bugfix-2.1.x n’ont pas été modifiées.

## État de compilation vérifié

Le workflow GitHub Actions « Mizar S Marlin 2.1.2.9 build » exécute `pio run -e STM32F103VE_GTM32_maple`.

- Run 42392936780, commit 58a006e4cef499c53757f1ed349218d530fe7479 : compilation PlatformIO réussie, artefact `firmware.bin` créé et publié. L’artefact GitHub Actions `mizar-s-marlin-2.1.2.9` expire le 2026-12-07.
- Ce run a confirmé les corrections LVGL des appels de labels et coordonnées, ainsi que le terminateur nul FR/EN.
- Avertissement du code du portage : `z_raised` inutilisé sous la configuration Mizar actuelle; ce checkpoint le conditionne aux chemins qui l’utilisent. La prochaine CI doit confirmer cette correction.
- Avertissements restants dans les journaux : bibliothèques/framework Maple, environnement Maple déprécié, et segment ELF RWX. Ils n’ont pas empêché la création de `firmware.bin`.
- La réussite de compilation ne valide ni les séquences réelles d’impression, ni l’écran, ni l’imprimante.

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

1. Confirmer que la CI du présent checkpoint reste verte; traiter toute nouvelle erreur révélée par les variantes de configuration.
2. Auditer les séquences LVGL ABL/MBL : démarrage, annulation, arrêt, fin, sauvegarde, retour et cohérence du nivellement actif.
3. Auditer le runout avec M600, réinsertion, purge et reprise du fichier.
4. Auditer PLR : sauvegarde EEPROM, détection PE3, fichier SD, reprise et effacement.
5. Intégrer ou documenter précisément le paquet d’images TFT et vérifier les chaînes FR/EN.
6. Conserver une procédure de flash et migration EEPROM; effectuer la validation sur banc avant livraison matérielle.

## Règle de livraison

Ne pas présenter le firmware comme prêt à flasher ou validé matériellement sans build réussi, revue statique des chemins concernés et essais réels de la carte.