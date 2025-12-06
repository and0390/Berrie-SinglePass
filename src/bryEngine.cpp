//
// Created by andreas on 11/26/25.
//
#include "bryEngine.h"
#include "bryObject.h"
#include "bryOpcode.h"
#include "utils.h"
#include "result.h"

BryEngine::BryEngine(Chunk *chunk)
    : gc_(*this), stack_(), variable_environment_(), string_table_(), chunk_(chunk), ip_(0)
{}

BryEngine::EngineStatus BryEngine::run() {
    for(;;) {
        Opcode instruction = static_cast<Opcode>(read_byte());
        switch (instruction) {
            case Opcode::ADD : {
                auto result = execute_binary_op(BryObject::add);
                if (result.has_value()) return result.value();
                break;
            }

            case Opcode::SUB : {
                auto result = execute_binary_op(BryObject::sub);
                if (result.has_value()) return result.value();
                break;
            }

            case Opcode::MUL : {
                auto result = execute_binary_op(BryObject::mul);
                if (result.has_value()) return result.value();
                break;
            }


            case Opcode::DIV : {
                auto result = execute_binary_op(BryObject::div);
                if (result.has_value()) return result.value();
                break;
            }

            case Opcode::EQUAL : {
                auto result = execute_binary_op(BryObject::eq);
                if (result.has_value()) return result.value();
                break;
            }

            case Opcode::LESS : {
                auto result = execute_binary_op(BryObject::ls);
                if (result.has_value()) return result.value();
                break;
            }

            case Opcode::GREATER : {
                auto result = execute_binary_op(BryObject::gt);
                if (result.has_value()) return result.value();
                break;
            }

            case Opcode::POS : {
                auto result = execute_unary_op(BryObject::pos);
                if (result.has_value()) return result.value();
                break;
            }

            case Opcode::NEG : {
                auto result = execute_unary_op(BryObject::neg);
                if (result.has_value()) return result.value();
                break;
            }

            case  Opcode::NOT : {
                auto result = execute_unary_op(BryObject::nt);
                if (result.has_value()) return result.value();

                break;
            }

            case Opcode::ECHO : {
                auto* value = stack_.pop();
                switch (value->get_type()) {
                    case BryObject::INT : {
                        fmt::print("{}", static_cast<BryInt*>(value)->get_value());
                        break;
                    }
                    case BryObject::FLOAT : {
                        fmt::print("{}", static_cast<BryFloat*>(value)->get_value());
                        break;
                    }
                    case BryObject::STRING : {
                        fmt::print("{}", static_cast<BryString*>(value)->get_value().c_str());
                        break;
                    }
                    case BryObject::BOOL : {
                        fmt::println("{}", static_cast<bool>(static_cast<BryBool*>(value)->get_value()));
                        break;
                    }
                    default: {
                        panic("Failed to print object, Unknown object.");
                    }
                }
                break;
            }

            case Opcode::LOAD_CONST : {
                std::uint16_t idx = read_index();
                stack_.push(chunk_->get_constantList().at(idx));
                break;
            }

            case Opcode::INIT_GLOBAL : {
                std::uint16_t idx = read_index();
                BryString* var_name = static_cast<BryString *>(chunk_->get_constantList().at(idx));
                assert(var_name && "in BryEngine::run (case INIT_GLOBAL) failed to convert BryString from variable environment");

                BryObject* obj = stack_.pop();

                if (variable_environment_.find(var_name) != variable_environment_.end()) {
                    print_error(fmt::format("variable {} has been defined", var_name->get_value().c_str()));
                    return RUNTIME_ERROR;
                }

                variable_environment_.insert({var_name, obj});
                break;
            }

            case Opcode::GET_GLOBAL : {
                std::uint16_t idx = read_index();
                BryString* var_name = static_cast<BryString *>(chunk_->get_constantList().at(idx));
                assert(var_name && "in BryEngine::run (case GET_GLOBAL) failed to convert BryString from variable environment");

                auto it = variable_environment_.find(var_name);
                if (it == variable_environment_.end()) {
                    print_error(fmt::format("variable {} is not defined", var_name->get_value().c_str()));
                    return RUNTIME_ERROR;
                }

                BryObject* obj = it->second;
                stack_.push(obj);
                break;
            }

            case Opcode::RETURN : {
                return OK;
            }

            default : {
                fmt::print("UNKNOWN\n");
                break;
            }
        }
    }
}

std::uint16_t BryEngine::read_index() noexcept {
    std::uint8_t low = read_byte();
	std::uint8_t high = read_byte();
	std::uint16_t i_reconstructed = (high << 8) | low;
	return i_reconstructed;
}

std::uint8_t BryEngine::read_byte() noexcept {
    std::uint8_t instruction = chunk_->get_codeList().at(ip_);
	ip_++;
	return instruction;
}

std::int32_t BryEngine::read_line() const noexcept {
    return chunk_->get_lineList().at(ip_);
}

void BryEngine::print_error(std::string_view msg) const {
    fmt::println("RuntimeError at line {} : {}.", read_line(), msg);
}

std::optional<BryEngine::EngineStatus> BryEngine::execute_binary_op(binaryfn call) {
    auto* rhs = stack_.pop();
    auto* lhs = stack_.pop();

    auto result = call(gc_, lhs, rhs);

    if (!result) {
        print_error(result.error());
        return RUNTIME_ERROR;
    }
    else {
        stack_.push(result.value());
        return std::nullopt;
    }
}

std::optional<BryEngine::EngineStatus> BryEngine::execute_unary_op(unaryfn call) {
    auto* rhs = stack_.pop();

    auto result = call(gc_, rhs);

    if (!result) {
        print_error(result.error());
        return RUNTIME_ERROR;
    }
    else {
        stack_.push(result.value());
        return std::nullopt;
    }

}

