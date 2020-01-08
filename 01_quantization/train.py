import argparse
import os
from typing import Tuple

import torch
import torch.nn as nn
import torch.optim as optim
import torchvision as tv
import torchvision.transforms as transforms
from torch.utils.data import DataLoader
from tqdm import tqdm

from model import LeNet


def train_epoch(
    model: nn.Module,
    loader: DataLoader,
    criterion: nn.Module,
    optimizer: optim.Optimizer,
    device: torch.device,
    desc: str = "Training"
) -> float:
    """Trains model for one epoch.

    Args:
        model: Neural network model.
        loader: DataLoader for training data.
        criterion: Loss function.
        optimizer: Optimization algorithm.
        device: Device to run training on.
        desc: Description for progress bar.

    Returns:
        Average loss for the epoch.
    """
    model.train()
    total_loss = 0.0
    
    pbar = tqdm(loader, desc=desc, leave=False)
    for i, (inputs, labels) in enumerate(pbar):
        inputs, labels = inputs.to(device), labels.to(device)
        
        optimizer.zero_grad()
        outputs = model(inputs)
        loss = criterion(outputs, labels)
        loss.backward()
        optimizer.step()
        
        total_loss += loss.item()
        pbar.set_postfix({'Loss': total_loss / (i + 1)})
        
    return total_loss / len(loader)


def evaluate(
    model: nn.Module,
    loader: DataLoader,
    device: torch.device
) -> float:
    """Evaluates model accuracy.

    Args:
        model: Neural network model.
        loader: DataLoader for test data.
        device: Device to run evaluation on.

    Returns:
        Accuracy as a percentage.
    """
    model.eval()
    correct = total = 0
    
    with torch.no_grad():
        pbar = tqdm(loader, desc="Evaluating", leave=False)
        for inputs, labels in pbar:
            inputs, labels = inputs.to(device), labels.to(device)
            outputs = model(inputs)
            _, predicted = outputs.max(1)
            
            total += labels.size(0)
            correct += (predicted == labels).sum().item()
            
            accuracy = 100.0 * correct / total
            pbar.set_postfix({'Accuracy': f"{accuracy:.2f}%"})
            
    return accuracy


def train_model(
    model: nn.Module,
    train_loader: DataLoader,
    test_loader: DataLoader,
    optimizer: optim.Optimizer,
    criterion: nn.Module,
    device: torch.device,
    num_epochs: int,
    desc_prefix: str = "Epoch"
) -> nn.Module:
    """Trains the model.

    Args:
        model: Neural network model.
        train_loader: DataLoader for training data.
        test_loader: DataLoader for test data.
        optimizer: Optimization algorithm.
        criterion: Loss function.
        device: Device to run on.
        num_epochs: Number of epochs to train.
        desc_prefix: Prefix for progress bar description.

    Returns:
        Trained model.
    """
    for epoch in range(num_epochs):
        train_epoch(
            model, train_loader, criterion, optimizer, device,
            desc=f"{desc_prefix} {epoch+1}/{num_epochs}")
        evaluate(model, test_loader, device)
    return model


def qat(
    model: nn.Module,
    train_loader: DataLoader,
    test_loader: DataLoader,
    optimizer: optim.Optimizer,
    criterion: nn.Module,
    device: torch.device,
    num_epochs: int
) -> nn.Module:
    """Performs quantization-aware training and converts to quantized model.

    Args:
        model: Model to quantize.
        train_loader: DataLoader for training data.
        test_loader: DataLoader for test data.
        optimizer: Optimization algorithm.
        criterion: Loss function.
        device: Device to run on.
        num_epochs: Number of QAT epochs.

    Returns:
        Quantized model.
    """
    model.use_quantization = True
    model.fuse_model()
    model.qconfig = torch.quantization.get_default_qat_qconfig('fbgemm')
    model = torch.quantization.prepare_qat(model, inplace=True)
    
    model = train_model(model, train_loader, test_loader, optimizer, criterion,
                       device, num_epochs, desc_prefix="QAT Epoch")
    
    model = model.cpu().eval()
    return torch.quantization.convert(model, inplace=True)


def get_dataloaders(
    batch_size: int
) -> Tuple[DataLoader, DataLoader]:
    """Creates MNIST training and test dataloaders.

    Args:
        batch_size: Number of samples per batch.

    Returns:
        Tuple of (train_loader, test_loader).
    """
    transform = transforms.ToTensor()
    train_set = tv.datasets.MNIST(
        root='./data/', train=True, download=True, transform=transform)
    test_set = tv.datasets.MNIST(
        root='./data/', train=False, download=True, transform=transform)
    
    train_loader = DataLoader(
        train_set, batch_size=batch_size, shuffle=True)
    test_loader = DataLoader(
        test_set, batch_size=batch_size, shuffle=False)
    
    return train_loader, test_loader


def main():
    """Main training function."""
    parser = argparse.ArgumentParser(description='Train LeNet on MNIST')
    parser.add_argument('--ckpt_dir', default='./ckpt',
                      help='checkpoint directory')
    parser.add_argument('--epochs', type=int, default=8,
                      help='number of epochs')
    parser.add_argument('--batch_size', type=int, default=128,
                      help='batch size')
    parser.add_argument('--lr', type=float, default=0.001,
                      help='learning rate')
    parser.add_argument('--qat_epochs', type=int, default=2,
                      help='quantization-aware training epochs')
    args = parser.parse_args()

    # Create checkpoint directory if it doesn't exist
    os.makedirs(args.ckpt_dir, exist_ok=True)

    # Setup
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    train_loader, test_loader = get_dataloaders(args.batch_size)
    model = LeNet().to(device)
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=args.lr)

    # Regular training
    print("Starting regular training...")
    model = train_model(
        model, train_loader, test_loader, optimizer, criterion,
        device, args.epochs
    )
    
    # Quantization-aware training
    print("\nStarting quantization-aware training...")
    quantized_model = qat(
        model, train_loader, test_loader, optimizer, criterion,
        device, args.qat_epochs
    )

    # Save model
    torch.save(
        {'model': quantized_model.state_dict()},
        f"{args.ckpt_dir}/quantized_model.pth"
    )


if __name__ == "__main__":
    main()