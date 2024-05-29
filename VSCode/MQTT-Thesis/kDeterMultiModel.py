import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.preprocessing import StandardScaler
import sklearn.neighbors as skl_nb
import sklearn.model_selection as skl_ms

# Load training data
radioMap = pd.read_csv("RadioMapMatrixCombined.txt", sep=" ")

# Extract features and target variables
X = radioMap.iloc[:, 1:-2]
y = radioMap.iloc[:, -2:]

# Standardize features
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

# Define different feature combinations
feature_combinations = {
    "Wi-Fi": X_scaled[:, :4],  # Only Wi-Fi
    "Wi-Fi + Mag": X_scaled[:, [0, 1, 2, 3, 9]],
    "Wi-Fi + BLE": X_scaled[:, :9],  # Wi-Fi + BLE
    "Wi-Fi + BLE + Magnetic": X_scaled[:, :10]  # Wi-Fi + BLE + Magnetic
}

# Define the range of k values
K_200 = np.arange(1, 201)
K_20 = np.arange(1, 21)

# Initialize dictionary to store misclassification rates for each feature combination
misclassification_200 = {key: np.zeros(len(K_200)) for key in feature_combinations}
misclassification_20 = {key: np.zeros(len(K_20)) for key in feature_combinations}

# Perform cross-validation for each feature combination for K up to 200
n_fold = 10
cv = skl_ms.KFold(n_splits=n_fold, random_state=2, shuffle=True)
for feature_name, X_features in feature_combinations.items():
    for train_index, val_index in cv.split(X_features):
        X_train, X_val = X_features[train_index], X_features[val_index]
        y_train, y_val = y.iloc[train_index], y.iloc[val_index]

        for j, k in enumerate(K_200):
            model = skl_nb.KNeighborsClassifier(n_neighbors=k)
            model.fit(X_train, y_train)
            prediction = model.predict(X_val)
            misclassification_200[feature_name][j] += np.mean(prediction != y_val.values)

        for j, k in enumerate(K_20):
            model = skl_nb.KNeighborsClassifier(n_neighbors=k)
            model.fit(X_train, y_train)
            prediction = model.predict(X_val)
            misclassification_20[feature_name][j] += np.mean(prediction != y_val.values)

# Average misclassification rates across folds
for feature_name in feature_combinations.keys():
    misclassification_200[feature_name] /= n_fold
    misclassification_20[feature_name] /= n_fold

# Plot misclassification rates for each feature combination
fig, ax = plt.subplots(1, 2, figsize=(16, 6))

for feature_name in feature_combinations.keys():
    ax[0].plot(K_200, misclassification_200[feature_name], label=feature_name)
    ax[1].plot(K_20, misclassification_20[feature_name], label=feature_name)

ax[0].set_title("10-fold Cross-validation error for kNN (k up to 200)")
ax[0].set_xlabel("Number of neighbors $k$", fontsize=12)
ax[0].set_ylabel("Validation error", fontsize=14)
ax[0].legend(fontsize=14)

ax[1].set_title("10-fold Cross-validation error for kNN (k up to 20)")
ax[1].set_xlabel("Number of neighbors $k$", fontsize=12)
ax[1].set_ylabel("Validation error", fontsize=12)
ax[1].legend(fontsize=14)

plt.tight_layout()
plt.show()
