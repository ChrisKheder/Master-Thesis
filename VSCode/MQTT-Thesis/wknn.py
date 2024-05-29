import pandas as pd
import numpy as np
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import KFold
from sklearn.metrics import mean_absolute_error, mean_squared_error, accuracy_score
from math import sqrt
import matplotlib.pyplot as plt
from scipy.spatial import distance

# Load training data
radioMap = pd.read_csv("RadioMapMatrixCombined.txt", sep=" ")

# Load test data
test = pd.read_csv("SaveMeasurement.txt", sep=" ")

# Separate features (RSSI values) from coordinates
X = radioMap.iloc[:, 1:-2]
y = radioMap.iloc[:, -2:]

# Separate features and coordinates for the test dataset
X_test = test.iloc[:, 1:-2]
y_test = test.iloc[:, -2:]

# Standardize features
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)
X_test_scaled = scaler.transform(X_test)  # Ensure the test set is scaled with the same scaler

# Define different feature combinations
feature_combinations = {
    "Wi-Fi": X_scaled[:, :4],  # Only Wi-Fi
    "Wi-Fi + Mag": X_scaled[:, [0, 1, 2, 3, 9]],
    "Wi-Fi + BLE": X_scaled[:, :9],  # Wi-Fi + BLE
    "Wi-Fi + BLE + Magnetic": X_scaled[:, :10]  # Wi-Fi + BLE + Magnetic
}

# Best k values for each feature combination
K = [2, 5, 4, 4]

# Initialize results dictionary
results = {}

epsilon = 1e-5  # Small value to prevent division by zero

# Function to predict using Weighted KNN
def weighted_knn_predict(X_train, y_train, X_test, k):
    predictions = []
    for x in X_test:
        # Compute distances to all points in the training set
        dists = np.array([distance.euclidean(x, x_train) for x_train in X_train])
        
        # Get the indices of the k nearest neighbors
        k_indices = np.argsort(dists)[:k]
        
        # Get the corresponding labels of the k nearest neighbors
        k_labels = y_train.iloc[k_indices].values
        
        # Compute the weights
        weights = 1 / (dists[k_indices] + epsilon)
        weights /= np.sum(weights)
        
        # Predict the label as the weighted average of the k nearest labels
        weighted_label = np.average(k_labels, axis=0, weights=weights)
        predictions.append(weighted_label)
        
    return np.array(predictions)

# Loop through each feature combination and corresponding k value
for (name, features), k in zip(feature_combinations.items(), K):
    print(f"Evaluating model: {name} with k={k}")
    
    # Define cross-validation
    n_fold = 10
    cv = KFold(n_splits=n_fold, random_state=2, shuffle=True)
    
    # Initialize metrics
    validation_mae = 0
    validation_rmse = 0
    
    # Cross-validation loop
    for train_index, val_index in cv.split(features):
        X_train, X_val = features[train_index], features[val_index]
        y_train, y_val = y.iloc[train_index], y.iloc[val_index]
        
        val_pred = weighted_knn_predict(X_train, y_train, X_val, k)
        
        # Validation predictions and MAE
        validation_mae += mean_absolute_error(y_val, val_pred)
        
        # Validation predictions and RMSE
        
        validation_rmse += sqrt(mean_squared_error(y_val, val_pred))
    
    # Average the errors across all folds
    validation_mae /= n_fold
    validation_rmse /= n_fold
    
    # Train the final model on the entire training dataset and evaluate on the test dataset
    y_test_pred = weighted_knn_predict(features, y, X_test_scaled[:, :features.shape[1]], k)
    
    # Calculate test metrics
    test_mae = mean_absolute_error(y_test, y_test_pred)
    test_rmse = sqrt(mean_squared_error(y_test, y_test_pred))
    
    # Calculate accuracy for each coordinate separately
    accuracies = [accuracy_score(y_test.iloc[:, i], np.round(y_test_pred[:, i])) for i in range(y_test.shape[1])]
    
    # Calculate mean accuracy across all coordinates
    mean_accuracy = np.mean(accuracies)
    
    # Store results
    results[name] = {
        "Training MAE": validation_mae,
        "Validation RMSE": validation_rmse,
        "Test MAE": test_mae,
        "Test RMSE": test_rmse,
        "Test Accuracy": mean_accuracy
    }
    
# Print results
for model_name, metrics in results.items():
    print(f"\nResults for {model_name}:")
    for metric_name, value in metrics.items():
        print(f"{metric_name}: {value}")

# Extracting the data for plotting
feature_names = list(results.keys())
validation_mae = [results[name]['Training MAE'] for name in feature_names]
validation_rmse = [results[name]['Validation RMSE'] for name in feature_names]
test_mae = [results[name]['Test MAE'] for name in feature_names]
test_rmse = [results[name]['Test RMSE'] for name in feature_names]
test_accuracy = [results[name]['Test Accuracy'] for name in feature_names]

# Plotting
fig, ax = plt.subplots(2, 2, figsize=(14, 10))

# Training MAE
ax[0, 0].bar(feature_names, validation_mae, color='skyblue')
ax[0, 0].set_title('Validation MAE')
ax[0, 0].set_ylabel('MAE')

# Validation RMSE
ax[0, 1].bar(feature_names, validation_rmse, color='lightgreen')
ax[0, 1].set_title('Validation RMSE')
ax[0, 1].set_ylabel('RMSE')

# Test MAE
ax[1, 0].bar(feature_names, test_mae, color='salmon')
ax[1, 0].set_title('Test MAE')
ax[1, 0].set_ylabel('MAE')

# Test RMSE
ax[1, 1].bar(feature_names, test_rmse, color='lightcoral')
ax[1, 1].set_title('Test RMSE')
ax[1, 1].set_ylabel('RMSE')

for axes in ax.flat:
    axes.set_xticklabels(feature_names, rotation=45, ha='right')

# Adding the main title for the entire figure
fig.suptitle('Model Performance Comparison for WkNN', fontsize=16)

plt.tight_layout(rect=[0, 0, 1, 0.96])  # Adjust layout to make room for the main title
plt.show()
