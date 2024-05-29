import pandas as pd
import numpy as np
from sklearn.preprocessing import StandardScaler
from sklearn.neighbors import KNeighborsClassifier
from sklearn.metrics import accuracy_score
from sklearn.metrics import mean_absolute_error
from sklearn.metrics import mean_squared_error
import sklearn.model_selection as skl_ms
from math import sqrt


# Load training data
radioMap = pd.read_csv("RadioMapMatrixCombined.txt",sep=" ")

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
    "Wi-Fi + Mag": X_scaled[:, [0] + [1] + [2]+ [3] + [9]],
    "Wi-Fi + BLE": X_scaled[:, :9],  # Wi-Fi + BLE
    "Wi-Fi + BLE + Magnetic": X_scaled[:, :10]  # Wi-Fi + BLE + Magnetic
}

# Best k values for each feature combination
K = [6, 6, 3, 5]

# Initialize results dictionary
results = {}

n_fold = 10
cv = skl_ms.KFold(n_splits = n_fold, random_state =2, shuffle=True)
misclassification = 0

training_mae = 0
validation_rmse = 0

# Loop through each feature combination and corresponding k value
for (name, features), k in zip(feature_combinations.items(), K):
    print(f"Evaluating model: {name} with k={k}")
    
    # Define cross-validation
    n_fold = 10
    cv = skl_ms.KFold(n_splits=n_fold, random_state=2, shuffle=True)
    
    # Initialize metrics
    training_mae = 0
    validation_rmse = 0
    
    # Cross-validation loop
    for train_index, val_index in cv.split(features):
        X_train, X_val = features[train_index], features[val_index]
        y_train, y_val = y.iloc[train_index], y.iloc[val_index]
        
        model = KNeighborsClassifier(n_neighbors=k)
        model.fit(X_train, y_train)
        
        # Training predictions and MAE
        train_pred = model.predict(X_train)
        training_mae += mean_absolute_error(y_train, train_pred)
        
        # Validation predictions and RMSE
        val_pred = model.predict(X_val)
        validation_rmse += sqrt(mean_squared_error(y_val, val_pred))
    
    # Average the errors across all folds
    training_mae /= n_fold
    validation_rmse /= n_fold
    
    # Train the final model on the entire training dataset
    final_model = KNeighborsClassifier(n_neighbors=k)
    final_model.fit(features, y)
    
    # Evaluate the model on the separate test dataset
    y_test_pred = final_model.predict(X_test_scaled[:, :features.shape[1]])
    
    # Calculate test metrics
    test_mae = mean_absolute_error(y_test, y_test_pred)
    test_rmse = sqrt(mean_squared_error(y_test, y_test_pred))
    
    # Calculate accuracy for each coordinate separately
    accuracies = [accuracy_score(y_test.iloc[:, i], y_test_pred[:, i]) for i in range(y_test.shape[1])]
    
    # Calculate mean accuracy across all coordinates
    mean_accuracy = np.mean(accuracies)
    
    # Store results
    results[name] = {
        "Training MAE": training_mae,
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
