import numpy as np
import matplotlib.pyplot as plt
from sklearn.datasets import make_blobs
from sklearn.neighbors import KNeighborsClassifier
from matplotlib.colors import ListedColormap

# Step 1: Generate synthetic data with varying scatter
# Circular blobs with different scatter
X1, y1 = make_blobs(n_samples=100, centers=[(-1, -1)], random_state=42, cluster_std=1)
X2, y2 = make_blobs(n_samples=100, centers=[(1, 1)], random_state=42, cluster_std=0.3)
X3, y3 = make_blobs(n_samples=100, centers=[(2, -2)], random_state=42, cluster_std=1.2)  # More scatter

# Combine the datasets
X = np.vstack((X1, X2, X3))
y = np.hstack((y1, y2 + 1, y3 + 2))

# Function to plot decision boundaries and visualize classification errors
def plot_decision_boundaries(X, y, k_values):
    # Define the color maps
    cmap_light = ListedColormap(['#FFAAAA', '#AAFFAA', '#AAAAFF'])
    cmap_bold = ListedColormap(['#FF0000', '#00FF00', '#0000FF'])

    x_min, x_max = X[:, 0].min() - 1, X[:, 0].max() + 1
    y_min, y_max = X[:, 1].min() - 1, X[:, 1].max() + 1
    xx, yy = np.meshgrid(np.arange(x_min, x_max, 0.1),
                         np.arange(y_min, y_max, 0.1))

    plt.figure(figsize=(18, 12))

    for i, k in enumerate(k_values):
        knn = KNeighborsClassifier(n_neighbors=k)
        knn.fit(X, y)

        Z = knn.predict(np.c_[xx.ravel(), yy.ravel()])
        Z = Z.reshape(xx.shape)

        plt.subplot(2, len(k_values) // 2, i + 1)
        plt.pcolormesh(xx, yy, Z, cmap=cmap_light)

        # Plot the training points with different markers for misclassified points
        y_pred = knn.predict(X)
        incorrect = y_pred != y
        plt.scatter(X[incorrect, 0], X[incorrect, 1], c='black', marker='x', s=50, label='Misclassified')
        plt.scatter(X[~incorrect, 0], X[~incorrect, 1], c=y[~incorrect], cmap=cmap_bold, edgecolor='k', s=20, label='Correctly Classified')

        # Count misclassifications and display the count below the legend
        misclassifications = np.sum(incorrect)
        plt.title(f'KNN with k={k}', fontsize=22)  # Enlarge title font size
        plt.xlabel('X1', fontsize=14)  # Enlarge x-axis label font size
        plt.ylabel('X2', fontsize=14)  # Enlarge y-axis label font size
        plt.xlim(xx.min(), xx.max())
        plt.ylim(yy.min(), yy.max())
        plt.legend(fontsize=15)  # Enlarge legend font size

        plt.text(xx.max() - 0.1, yy.min() + 0.1, f'Misclassifications: {misclassifications}', color='black', fontsize=18, horizontalalignment='right')

    plt.tight_layout()
    plt.show()

# Step 2: Define different k values to plot
k_values = [1, 5, 10, 100]

# Step 3: Plot decision boundaries for each k
plot_decision_boundaries(X, y, k_values)
