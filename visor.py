import matplotlib.pyplot as plt
import matplotlib.image as mpimg

img = mpimg.imread('zona_seleccionada.ppm')
plt.figure(figsize=(6,6), frameon=False)  # ventana cuadrada sin bordes extra
plt.imshow(img)
plt.axis('off')
plt.tight_layout(pad=0)
plt.show()
