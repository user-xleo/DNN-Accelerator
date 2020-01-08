"""LeNet model implementation with quantization support."""

import torch
import torch.nn as nn


class LeNet(nn.Module):
    """LeNet model for MNIST digit classification with optional quantization."""

    def __init__(self, use_quantization: bool = False):
        """Initializes the LeNet model.

        Args:
            use_quantization: If True, enables quantization-aware training.
        """
        super().__init__()
        self.use_quantization = use_quantization
        
        self.conv1 = nn.Sequential(
            nn.Conv2d(1, 6, 5, padding=2),
            nn.ReLU(),
            nn.MaxPool2d(2)
        )
        
        self.conv2 = nn.Sequential(
            nn.Conv2d(6, 16, 5),
            nn.ReLU(),
            nn.MaxPool2d(2)
        )

        self.fc1 = nn.Sequential(
            nn.Linear(16 * 5 * 5, 120),
            nn.ReLU()
        )
        
        self.fc2 = nn.Sequential(
            nn.Linear(120, 84),
            nn.ReLU()
        )

        self.fc3 = nn.Linear(84, 10)
        
        # Quantization layers
        self.quant = torch.quantization.QuantStub()
        self.dequant = torch.quantization.DeQuantStub()

    def forward(self, input_tensor: torch.Tensor) -> torch.Tensor:
        """Performs forward pass through the network.
        
        Args:
            input_tensor: Input image tensor of shape [batch_size, 1, 28, 28]
            
        Returns:
            Output logits of shape [batch_size, 10]
        """
        if self.use_quantization:
            features = self.quant(input_tensor)
        else:
            features = input_tensor
        
        # First conv block
        conv1_out = self.conv1(features)
        
        # Second conv block
        conv2_out = self.conv2(conv1_out)
        
        # Flatten for FC layers
        batch_size = conv2_out.size(0)
        flattened = conv2_out.view(batch_size, -1)
        
        # Fully connected blocks
        fc1_out = self.fc1(flattened)
        fc2_out = self.fc2(fc1_out)
        logits = self.fc3(fc2_out)
        
        if self.use_quantization:
            logits = self.dequant(logits)
        return logits

    def fuse_model(self):
        """Fuses Conv2d+ReLU and Linear+ReLU layers for quantization."""
        # Fuse conv blocks
        torch.quantization.fuse_modules(
            self.conv1, ['0', '1'], inplace=True)
        torch.quantization.fuse_modules(
            self.conv2, ['0', '1'], inplace=True)
        
        # Fuse fc blocks
        torch.quantization.fuse_modules(
            self.fc1, ['0', '1'], inplace=True)
        torch.quantization.fuse_modules(
            self.fc2, ['0', '1'], inplace=True)