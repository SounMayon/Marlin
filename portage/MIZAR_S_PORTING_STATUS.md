# Geeetech Mizar S — suivi de portage Marlin 2.1.2.9

Dernière mise à jour : 2026-11-07 (Europe/Paris)

## Cible et limites

- Base : MarlinFirmware/Marlin 2.1.2.9, commit 7d300b42acadb3e7467e9f4b4fc03ebea592c2d0.
- Branche de travail : mizar-s-marlin-2.1.2.9.
- Branche auditée avant ce checkpoint : bed50dfd5d9c2c5a2474cb254da7b9f81047cbad.
- Les branches main et bugfix-2.1.x n’ont pas été modifiées.

## État de compilation vérifié

Le workflow GitHub Actions « Mizar S Marlin 2.1.2.9 build » exécute `pio run -e STM32F103VE_GTM32_maple`.

- Run 42392936780, commit 58a006e4cef499c53757f1ed349218d530fe7479 : compilation réussie et artefact `firmware.bin` publié. Ce run a révélé un avertissement `z_raised` inutilisé.
- Run 42393229867, commit bed50dfd5d9c2c5a2474cb254da7b9f81047cbad : compilation réussie après le garde conditionnel de `z_raised`; aucun avertissement du code Mizar n’apparaît dans les journaux.
- Les avertissements restants concernent le framework Maple, l’environnement Maple déprécié et le segment ELF RWX. Ils n’ont pas empêché la production de `firmware.bin`.
- La compilation a été exécutée par GitHub Actions. Elle ne valide ni le fonctionnement de l’écran, ni les capteurs, ni la reprise d’impression sur la machine.

## Travail déjà intégré

- Carte GTM32/Mizar S et environnement STM32F103VE_GTM32_maple, avec script de relocation bootloader.
- Dual leveling ABL/MBL : objets et grilles distincts, sélection runtime, routage G-code, planificateur et persistance EEPROM V89.
- M421 MBL lit les index en 32 bits puis les valide avant l’accès au maillage.
- Écrans LVGL auto et manuel 4×4, sélection de mode, sauvegarde/restauration des deux maillages. Revue statique des chemins d’arrêt et retour : l’ABL restaure le maillage précédent après l’abandon; le MBL annule en restaurant sa copie et sauvegarde le maillage terminé.
- Le code de nivellement automatique vérifie le drapeau d’arrêt avant chaque nouveau point de sonde; un arrêt n’interrompt donc pas un mouvement de sonde déjà engagé.
- La persistance EEPROM V89 conserve le mode sélectionné et les données ABL/MBL; le build courant valide la configuration cible.

## Runout, PLR et ressources

- Configuration LVGL : `MT_DET_1_PIN` et `FIL_RUNOUT_PIN` partagent PA15; le capteur Marlin natif reste désactivé pour éviter le double déclenchement. Le handler MKS échantillonne le signal actif-bas, lance une pause SD et le polling attend la fin du mouvement avant de parquer la tête. La reprise restaure XY/Z puis relance M24.
- Ce chemin ne lance pas automatiquement M600, une purge ou un chargement; le geste de réinsertion/reprise est manuel depuis l’écran. La polarité et le comportement électrique restent à tester sur la carte.
- Variante sans LVGL : le pin de runout est PA4, également déclaré pour TOUCH_CS dans la définition de carte; ne pas utiliser cette variante sans résoudre cette affectation.
- PLR est activé avec entrée PE3 active-bas, sans pull-up/pull-down interne configuré tant que le circuit de détection n’est pas confirmé. Les données de reprise sont stockées après SettingsData avec contrôle CRC et vérification de taille; la séquence complète coupure/redémarrage/SD/reprise n’a pas été essayée.
- Les 120 noms d’images `.bin` du gestionnaire MKS ont tous un JPG de même nom de base dans `SounMayon/Mizar-S/Resource/icon` (révision examinée : `d3656d6db4bb89f552f9afd3aec013db02a1cff9`). Le dépôt firmware ne contient pas les binaires TFT; le convertisseur constructeur indique que sa sortie `.bin` n’a pas été éprouvée. Détails et blocage dans `portage/mizar-s/TFT_ASSETS.md`.
- Les chaînes de nivellement spécifiques sont fournies en français/anglais et la configuration Mizar normalise les langues vers ces deux choix.
- Aucun essai matériel de runout, perte secteur, migration EEPROM, affichage des icônes ou impression n’est disponible.

## À terminer

1. Convertir les 120 JPG en images MKS `.bin` avec un format vérifié, intégrer le paquet TFT et contrôler l’affichage de toutes les icônes.
2. Tester sur carte le runout actif-bas, la réinsertion, l’action manuelle de chargement/purge et la reprise du fichier.
3. Tester ABL et MBL sur l’écran : démarrage, arrêt, annulation, fin, retour et activation du bon maillage.
4. Tester PE3 et toute la reprise PLR (sauvegarde EEPROM, coupure, détection, fichier SD, reprise et effacement).
5. Documenter la procédure de flash et de migration EEPROM, puis effectuer les validations sur banc et imprimante.

## Règle de livraison

Ne pas présenter le firmware comme prêt à flasher ou validé matériellement sans build réussi, revue statique des chemins concernés et essais réels de la carte.