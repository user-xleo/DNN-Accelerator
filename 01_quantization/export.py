import argparse
import json
from typing import Dict, List, Optional, Any, Union

import torch
from model import LeNet


class ModelExporter:
    """A class to handle the export of PyTorch models to JSON format.
    
    This class provides methods to extract model architecture information and weights,
    handling both regular and quantized models.
    
    Attributes:
        model: The PyTorch model to be exported.
        state_dict: The state dictionary containing model parameters.
    """

    def __init__(self, model: torch.nn.Module, state_dict: Dict[str, torch.Tensor]):
        """Initializes the ModelExporter with a model and its state dictionary.

        Args:
            model: PyTorch model to be exported.
            state_dict: State dictionary containing model parameters.
        """
        self.model = model
        self.state_dict = state_dict

    def _process_layer(self, name: str, module: torch.nn.Module) -> Optional[Dict[str, Any]]:
        """Processes a single layer and returns its information.

        Args:
            name: Name of the layer.
            module: The layer module to process.

        Returns:
            A dictionary containing layer information or None if layer type is unknown.
        """
        if isinstance(module, torch.quantization.stubs.QuantStub):
            print(f"Found QuantStub: {name}")
            return {
                "name": name,
                "type": "QuantStub"
            }
            
        if isinstance(module, torch.nn.Conv2d):
            print(f"Found Conv2d: {name}")
            return {
                "name": name,
                "type": "Conv2d",
                "in_channels": module.in_channels,
                "out_channels": module.out_channels,
                "kernel_size": module.kernel_size[0],  # Assuming square kernel
                "stride": module.stride[0],  # Assuming square stride
                "padding": module.padding[0],  # Assuming square padding
            }
            
        if isinstance(module, torch.nn.Linear):
            print(f"Found Linear: {name}")
            return {
                "name": name,
                "type": "Linear",
                "in_features": module.in_features,
                "out_features": module.out_features,
            }
            
        if isinstance(module, torch.nn.ReLU):
            print(f"Found ReLU: {name}")
            return {
                "name": name,
                "type": "ReLU",
                "inplace": module.inplace
            }
            
        if isinstance(module, torch.nn.MaxPool2d):
            print(f"Found MaxPool2d: {name}")
            return {
                "name": name,
                "type": "MaxPool2d",
                "kernel_size": self._get_scalar_value(module.kernel_size),
                "stride": self._get_scalar_value(module.stride),
                "padding": self._get_scalar_value(module.padding),
            }

        if isinstance(module, torch.quantization.stubs.DeQuantStub):
            print(f"Found DeQuantStub: {name}")
            return {
                "name": name,
                "type": "DeQuantStub"
            }
            
        print(f"Skipping unknown layer type: {type(module)}")
        return None

    @staticmethod
    def _get_scalar_value(param: Union[int, tuple]) -> int:
        """Extracts scalar value from potentially tuple parameters.

        Args:
            param: Parameter that could be either int or tuple.

        Returns:
            The scalar value from the parameter.
        """
        return param if isinstance(param, int) else param[0]

    def _get_model_info(self) -> List[Dict[str, Any]]:
        """Extracts model structure information.

        Returns:
            List of dictionaries containing layer information.
        """
        model_info = []
        
        print("\nExtracting model structure:")
        # First, process QuantStub
        for name, module in self.model.named_modules():
            if isinstance(module, torch.quantization.stubs.QuantStub):
                layer_info = self._process_layer(name, module)
                if layer_info:
                    model_info.append(layer_info)
                    break
        
        # Process other layers
        for name, module in self.model.named_children():
            if isinstance(module, torch.nn.Sequential):
                print(f"Processing Sequential block: {name}")
                for sub_name, sub_module in module.named_children():
                    if not isinstance(sub_module, torch.quantization.stubs.QuantStub):
                        layer_info = self._process_layer(f"{name}.{sub_name}", sub_module)
                        if layer_info:
                            model_info.append(layer_info)
            elif not isinstance(module, torch.quantization.stubs.QuantStub):
                layer_info = self._process_layer(name, module)
                if layer_info:
                    model_info.append(layer_info)
        
        return model_info

    def _process_tensor(self, tensor: torch.Tensor) -> Dict[str, Any]:
        """Processes a tensor and returns its information including quantization details.

        Args:
            tensor: The tensor to process.

        Returns:
            Dictionary containing tensor information and values.
        """
        tensor_info = {
            "shape": list(tensor.shape),
            "dtype": str(tensor.dtype)
        }

        if tensor.dtype in [torch.qint8, torch.quint8]:
            if tensor.qscheme() == torch.per_tensor_affine:
                values = tensor.int_repr().detach().numpy()
                tensor_info.update({
                    "quantization": "per_tensor",
                    "scale": float(tensor.q_scale()),
                    "values": values.tolist()
                })
            elif tensor.qscheme() == torch.per_channel_affine:
                values = tensor.int_repr().detach().numpy()
                tensor_info.update({
                    "quantization": "per_channel",
                    "scales": tensor.q_per_channel_scales().detach().numpy().tolist(),
                    "axis": int(tensor.q_per_channel_axis()),
                    "values": values.tolist()
                })
        else:
            values = tensor.detach().numpy()
            tensor_info.update({
                "quantization": "none",
                "values": values.tolist()
            })
        
        return tensor_info

    def export_to_json(self, output_path: str) -> None:
        """Exports model structure and weights to a JSON file.

        Args:
            output_path: Path where the JSON file will be saved.
        """
        model_info = self._get_model_info()
        
        # Process weights and biases for each layer
        last_scale = None
        for layer in model_info:
            name = layer["name"]
            self._process_layer_parameters(layer, name)
            
            # Store the scale from the last regular layer
            if layer["type"] not in ["QuantStub", "DeQuantStub"] and "scale" in layer:
                last_scale = layer["scale"]
            
            # Use the last layer's scale as dequant scale
            if layer["type"] == "DeQuantStub" and last_scale is not None:
                layer["scale"] = last_scale
        
        # Create final data structure and save
        data = {"layers": model_info}
        with open(output_path, 'w') as f:
            json.dump(data, f, indent=2)

    def _process_layer_parameters(self, layer: Dict[str, Any], name: str) -> None:
        """Processes and adds parameter information to a layer.

        Args:
            layer: Dictionary containing layer information.
            name: Name of the layer.
        """
        # Handle weights
        weight_key = f"{name}.weight"
        packed_weight_key = f"{name}._packed_params.weight"
        
        if weight_key in self.state_dict:
            layer["weight"] = self._process_tensor(self.state_dict[weight_key])
        elif packed_weight_key in self.state_dict:
            print(f"Found packed weight for {name}")
            layer["weight"] = self._process_tensor(self.state_dict[packed_weight_key])

        # Handle biases
        bias_key = f"{name}.bias"
        packed_bias_key = f"{name}._packed_params.bias"
        
        if bias_key in self.state_dict:
            layer["bias"] = self._process_tensor(self.state_dict[bias_key])
        elif packed_bias_key in self.state_dict:
            print(f"Found packed bias for {name}")
            layer["bias"] = self._process_tensor(self.state_dict[packed_bias_key])

        # Handle QuantStub and DeQuantStub parameters
        scale_key = f"{name}.scale"
        
        if scale_key in self.state_dict:
            layer["scale"] = float(self.state_dict[scale_key])

def main():
    """Main function to handle command line arguments and model export."""
    parser = argparse.ArgumentParser(description='Export model structure and weights')
    parser.add_argument('--ckpt_path', 
                       default='./ckpt/quantized_model.pth',
                       help='path to checkpoint file')
    parser.add_argument('--output_path', 
                       default='./LeNet.json',
                       help='path to output JSON file')
    args = parser.parse_args()

    # Create raw model and load checkpoint
    model = LeNet()
    print(f"Loading checkpoint from {args.ckpt_path}")
    checkpoint = torch.load(args.ckpt_path, map_location='cpu')
    
    # Export model
    exporter = ModelExporter(model, checkpoint['model'])
    exporter.export_to_json(args.output_path)
    print(f"Model exported to {args.output_path}")


if __name__ == '__main__':
    main()