# Audit des ressources TFT Mizar S

## Résultat

- Le gestionnaire d’images MKS de la branche `mizar-s-marlin-2.1.2.9` référence 120 noms distincts `F:/bmp_*.bin`.
- Les 120 noms ont un fichier JPG homonyme dans `SounMayon/Mizar-S/Resource/icon`, révision examinée `d3656d6db4bb89f552f9afd3aec013db02a1cff9`.
- Aucun paquet d’images TFT `.bin` n’est suivi dans la branche Marlin. La compilation du firmware ne produit pas ces ressources : elles sont lues par l’interface MKS depuis la mémoire SPI externe de l’écran.
- Le constructeur fournit un convertisseur LVGL dans `Resource/Lvgl_image_convert_tool`. Son README indique que la sortie `.bin` n’a pas été utilisée ou vérifiée par son auteur.

## Blocage avant intégration

Les JPG sont disponibles, mais le format de sortie doit être confirmé avec le décodeur MKS/LVGL et la procédure d’écriture de la mémoire SPI. Ne pas présenter les JPG ou une conversion non vérifiée comme un paquet TFT prêt à installer.

Pour terminer : convertir les 120 fichiers avec le format adéquat, vérifier les noms/couleurs/dimensions, installer le paquet sur l’écran et contrôler les vues FR/EN sur le matériel.
